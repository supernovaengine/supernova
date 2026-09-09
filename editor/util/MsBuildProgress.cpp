// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "MsBuildProgress.h"

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <fstream>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace doriax::editor {

namespace {
    namespace fs = std::filesystem;

    struct Timestamp {
        bool exists = false;
        fs::file_time_type value{};
    };

    struct Dependencies {
        bool complete = true;
        fs::file_time_type newest = fs::file_time_type::min();
    };

    std::string readFile(const fs::path& path) {
        std::ifstream stream(path, std::ios::binary);
        return stream
            ? std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>())
            : std::string();
    }

    std::string lowerAscii(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    std::wstring pathKey(const fs::path& path) {
        std::wstring key = path.lexically_normal().wstring();
        std::transform(key.begin(), key.end(), key.begin(),
                       [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
        return key;
    }

    fs::path resolve(const fs::path& projectFile, const std::string& value) {
        fs::path path = fs::u8path(value);
        return (path.is_relative() ? projectFile.parent_path() / path : path).lexically_normal();
    }

    std::string decodeXml(std::string value) {
        size_t pos = 0;
        while ((pos = value.find("&amp;", pos)) != std::string::npos) {
            value.replace(pos, 5, "&");
            ++pos;
        }
        return value;
    }

    std::string attributeValue(const std::string& xml, size_t begin, size_t end,
                               const std::string& attribute) {
        const std::string needle = attribute + "=\"";
        const size_t valuePos = xml.find(needle, begin);
        if (valuePos == std::string::npos || valuePos >= end) return {};
        const size_t valueBegin = valuePos + needle.size();
        const size_t valueEnd = xml.find('"', valueBegin);
        return valueEnd != std::string::npos && valueEnd <= end
            ? decodeXml(xml.substr(valueBegin, valueEnd - valueBegin))
            : std::string();
    }

    std::vector<std::string> attributeValues(const std::string& xml, const std::string& tag,
                                             const std::string& attribute) {
        std::vector<std::string> values;
        const std::string needle = "<" + tag;
        size_t pos = 0;
        while ((pos = xml.find(needle, pos)) != std::string::npos) {
            const size_t end = xml.find('>', pos + needle.size());
            if (end == std::string::npos) break;
            std::string value = attributeValue(xml, pos, end, attribute);
            if (!value.empty()) values.push_back(std::move(value));
            pos = end + 1;
        }
        return values;
    }

    std::string elementValue(const std::string& xml, size_t begin, size_t end,
                             const std::string& tag) {
        const size_t tagBegin = xml.find("<" + tag, begin);
        if (tagBegin == std::string::npos || tagBegin >= end) return {};
        const size_t valueBeginTag = xml.find('>', tagBegin);
        if (valueBeginTag == std::string::npos || valueBeginTag >= end) return {};
        const size_t valueEnd = xml.find("</" + tag + ">", valueBeginTag + 1);
        return valueEnd != std::string::npos && valueEnd < end
            ? decodeXml(xml.substr(valueBeginTag + 1, valueEnd - valueBeginTag - 1))
            : std::string();
    }

    void replaceAll(std::string& value, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = value.find(from, pos)) != std::string::npos) {
            value.replace(pos, from.size(), to);
            pos += to.size();
        }
    }

    std::string intermediateDir(const std::string& xml, const std::string& configuration,
                                const fs::path& projectFile) {
        const std::string condition = "=='" + configuration + "|";
        size_t pos = 0;
        while ((pos = xml.find("<IntDir", pos)) != std::string::npos) {
            const size_t tagEnd = xml.find('>', pos + 7);
            if (tagEnd == std::string::npos) break;
            if (xml.substr(pos, tagEnd - pos).find(condition) != std::string::npos) {
                const size_t valueEnd = xml.find("</IntDir>", tagEnd + 1);
                if (valueEnd != std::string::npos) {
                    return decodeXml(xml.substr(tagEnd + 1, valueEnd - tagEnd - 1));
                }
            }
            pos = tagEnd + 1;
        }
        return projectFile.stem().string() + ".dir/" + configuration;
    }

    std::string sourceFilename(std::string line) {
        const size_t begin = line.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) return {};
        line = line.substr(begin, line.find_last_not_of(" \t\r\n") - begin + 1);

        const size_t prefixEnd = line.find('>');
        if (prefixEnd != std::string::npos && prefixEnd > 0
            && std::all_of(line.begin(), line.begin() + prefixEnd,
                           [](unsigned char c) { return std::isdigit(c); })) {
            line.erase(0, prefixEnd + 1);
            const size_t content = line.find_first_not_of(" \t");
            if (content == std::string::npos) return {};
            line.erase(0, content);
        }
        return lowerAscii(line);
    }
}

class MsBuildProgress::Impl {
private:
    struct SourceCounts {
        size_t remaining = 0;
        size_t pending = 0;
    };

    size_t sourceCount = 0;
    size_t pendingCount = 0;
    size_t completedCount = 0;
    size_t observedCount = 0;
    bool observedFallback = false;
    float reportedFraction = 0.0f;
    std::unordered_map<std::string, SourceCounts> sourcesByName;
    std::unordered_map<std::wstring, Timestamp> timestamps;

    Timestamp timestamp(const fs::path& path) {
        const std::wstring key = pathKey(path);
        const auto cached = timestamps.find(key);
        if (cached != timestamps.end()) return cached->second;

        std::error_code ec;
        Timestamp result;
        result.value = fs::last_write_time(path, ec);
        result.exists = !ec;
        timestamps.emplace(key, result);
        return result;
    }

    std::unordered_map<std::wstring, Dependencies> readDependencies(const fs::path& path) {
        std::unordered_map<std::wstring, Dependencies> records;
        const std::string bytes = readFile(path);
        if (bytes.size() < 2
            || static_cast<unsigned char>(bytes[0]) != 0xff
            || static_cast<unsigned char>(bytes[1]) != 0xfe) {
            return records;
        }

        std::vector<std::wstring> sources;
        Dependencies dependencies;
        auto flush = [&]() {
            for (const std::wstring& source : sources) {
                Dependencies& record = records[source];
                record.complete = record.complete && dependencies.complete;
                record.newest = std::max(record.newest, dependencies.newest);
            }
            sources.clear();
            dependencies = Dependencies{};
        };

        auto consume = [&](std::wstring line) {
            if (!line.empty() && line.back() == L'\r') line.pop_back();
            if (line.empty()) return;
            if (line[0] == L'^') {
                flush();
                size_t begin = 1;
                while (begin <= line.size()) {
                    const size_t separator = line.find(L'|', begin);
                    const size_t end = separator == std::wstring::npos ? line.size() : separator;
                    if (end > begin) sources.push_back(pathKey(fs::path(line.substr(begin, end - begin))));
                    if (separator == std::wstring::npos) break;
                    begin = separator + 1;
                }
            } else if (!sources.empty()) {
                const Timestamp input = timestamp(fs::path(line));
                dependencies.complete = dependencies.complete && input.exists;
                if (input.exists) dependencies.newest = std::max(dependencies.newest, input.value);
            }
        };

        std::wstring line;
        for (size_t i = 2; i + 1 < bytes.size(); i += 2) {
            const wchar_t codeUnit = static_cast<wchar_t>(
                static_cast<unsigned char>(bytes[i])
                | (static_cast<unsigned char>(bytes[i + 1]) << 8));
            if (codeUnit == L'\n') {
                consume(std::move(line));
                line.clear();
            } else {
                line.push_back(codeUnit);
            }
        }
        if (!line.empty()) consume(std::move(line));
        flush();
        return records;
    }

    void addProject(const fs::path& projectFile, const std::string& xml,
                    const std::string& configuration) {
        const std::string intermediate = intermediateDir(xml, configuration, projectFile);
        const fs::path objectDir = resolve(projectFile, intermediate);
        const Timestamp projectTime = timestamp(projectFile);
        const auto dependencyRecords = readDependencies(
            objectDir / (projectFile.stem().string() + ".tlog") / "CL.read.1.tlog");

        size_t pos = 0;
        while ((pos = xml.find("<ClCompile", pos)) != std::string::npos) {
            const size_t tagEnd = xml.find('>', pos + 10);
            if (tagEnd == std::string::npos) break;
            const std::string include = attributeValue(xml, pos, tagEnd, "Include");
            if (include.empty()) {
                pos = tagEnd + 1;
                continue;
            }

            const fs::path source = resolve(projectFile, include);
            fs::path object = objectDir / (source.stem().string() + ".obj");
            if (tagEnd > pos && xml[tagEnd - 1] != '/') {
                const size_t itemEnd = xml.find("</ClCompile>", tagEnd + 1);
                if (itemEnd != std::string::npos) {
                    std::string objectValue = elementValue(xml, tagEnd + 1, itemEnd, "ObjectFileName");
                    if (!objectValue.empty()) {
                        replaceAll(objectValue, "$(IntDir)", intermediate);
                        object = resolve(projectFile, objectValue);
                    }
                }
            }

            ++sourceCount;
            const std::string name = lowerAscii(source.filename().string());
            SourceCounts& counts = sourcesByName[name];
            ++counts.remaining;

            const Timestamp sourceTime = timestamp(source);
            const Timestamp objectTime = timestamp(object);
            const auto tracked = dependencyRecords.find(pathKey(source));
            const bool cached = sourceTime.exists && objectTime.exists && projectTime.exists
                && tracked != dependencyRecords.end() && tracked->second.complete
                && objectTime.value >= sourceTime.value
                && objectTime.value >= projectTime.value
                && objectTime.value >= tracked->second.newest;
            if (!cached) {
                ++pendingCount;
                ++counts.pending;
            }
            pos = tagEnd + 1;
        }
    }

    float fraction() {
        const float predicted = pendingCount > 0
            ? static_cast<float>(completedCount) / static_cast<float>(pendingCount)
            : 0.0f;
        const float observed = observedFallback
            ? static_cast<float>(observedCount) / static_cast<float>(sourceCount)
            : 0.0f;
        const float candidate = std::min(std::max(predicted, observed), 1.0f);
        reportedFraction = std::max(reportedFraction, candidate);
        return reportedFraction;
    }

public:
    void initialize(const fs::path& buildDir, const std::string& configuration) {
        if (readFile(buildDir / "CMakeCache.txt").find(
                "CMAKE_GENERATOR:INTERNAL=Visual Studio") == std::string::npos) {
            return;
        }

        const fs::path allBuild = buildDir / "ALL_BUILD.vcxproj";
        std::error_code ec;
        if (!fs::is_regular_file(allBuild, ec)) return;

        std::vector<fs::path> projects{allBuild};
        std::unordered_set<std::wstring> visited;
        for (size_t i = 0; i < projects.size(); ++i) {
            const fs::path projectFile = projects[i];
            if (!visited.insert(pathKey(projectFile)).second) continue;

            const std::string xml = readFile(projectFile);
            if (xml.empty()) continue;
            addProject(projectFile, xml, configuration);
            for (const std::string& include : attributeValues(xml, "ProjectReference", "Include")) {
                projects.push_back(resolve(projectFile, include));
            }
        }
    }

    bool consumeLine(const std::string& line, float& result) {
        const std::string name = sourceFilename(line);
        auto source = sourcesByName.find(name);
        if (name.empty() || source == sourcesByName.end() || source->second.remaining == 0) {
            return false;
        }
        --source->second.remaining;
        ++observedCount;

        if (source->second.pending > 0) {
            --source->second.pending;
            ++completedCount;
        } else {
            observedFallback = true;
        }
        if (source->second.remaining == 0) sourcesByName.erase(source);
        result = fraction();
        return true;
    }
};

MsBuildProgress::MsBuildProgress(const fs::path& buildDir, const std::string& configuration)
    : impl(std::make_unique<Impl>()) {
    impl->initialize(buildDir, configuration);
}

MsBuildProgress::~MsBuildProgress() = default;

bool MsBuildProgress::consumeLine(const std::string& line, float& fraction) {
    return impl->consumeLine(line, fraction);
}

}
