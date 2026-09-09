// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ResourcePack.h"

#include "System.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
#include <mutex>
#ifndef _WIN32
#include <sys/types.h>
#endif

using namespace doriax;

namespace {

    constexpr char PACK_MAGIC[] = {'D', 'X', 'P', 'K', '1'};
    constexpr const char* PACK_FILENAME = "resources.pak";

#ifdef _WIN32
    typedef __int64 pack_offset_t;
#else
    typedef off_t pack_offset_t;
#endif

    // Still 32-bit on the armeabi-v7a and x86 ABIs, so a larger pack is rejected
    constexpr uint64_t MAX_PACK_OFFSET = static_cast<uint64_t>(std::numeric_limits<pack_offset_t>::max());

    struct PackEntry {
        uint64_t offset = 0;
        uint64_t size = 0;
        uint8_t key = 0;
        uint32_t shift = 0;
    };

    struct PackState {
        std::string filename;
        std::map<std::string, PackEntry> entries;
    };

    // Same static-destruction guard as TextureDataPool::getMap()
    PackState& state() {
        static PackState* packState = new PackState();
        return *packState;
    }

    // Filled once, then read-only, so lookups take no lock
    std::once_flag loadFlag;

    // Keys are '/'-separated and relative to the export root. "." and ".." resolve
    // like FileData::simplifyPath, so a glTF "models/../textures/a.png" still hits.
    std::string normalizePath(std::string path) {
        std::replace(path.begin(), path.end(), '\\', '/');

        std::vector<std::string> parts;
        for (size_t start = 0; start <= path.size(); ) {
            const size_t end = path.find('/', start);
            const std::string part = path.substr(start, end == std::string::npos ? std::string::npos : end - start);

            if (part == "..") {
                if (!parts.empty()) parts.pop_back();
            } else if (!part.empty() && part != ".") {
                parts.push_back(part);
            }

            if (end == std::string::npos) break;
            start = end + 1;
        }

        std::string result;
        for (size_t i = 0; i < parts.size(); i++) {
            if (i > 0) result += '/';
            result += parts[i];
        }
        return result;
    }

    bool startsWith(const std::string& value, const std::string& prefix) {
        return value.rfind(prefix, 0) == 0;
    }

    // Follows FileData::getSystemPath, but only the two packed roots have keys
    std::string getPackKey(std::string path) {
        std::replace(path.begin(), path.end(), '\\', '/');
        if (path.empty()) return "";

        if (startsWith(path, "asset://")) return "assets/" + normalizePath(path.substr(8));
        if (startsWith(path, "lua://")) return "lua/" + normalizePath(path.substr(6));

        if (path.find("://") != std::string::npos) return "";
        if (path.front() == '/' || (path.size() > 1 && path[1] == ':')) return "";

        return "assets/" + normalizePath(path);
    }

    bool readExact(FILE* file, void* dst, size_t size) {
        return std::fread(dst, 1, size, file) == size;
    }

    bool readU8(FILE* file, uint8_t& value) {
        return readExact(file, &value, 1);
    }

    bool readU16(FILE* file, uint16_t& value) {
        unsigned char bytes[2];
        if (!readExact(file, bytes, sizeof(bytes))) return false;
        value = static_cast<uint16_t>(bytes[0])
              | (static_cast<uint16_t>(bytes[1]) << 8);
        return true;
    }

    bool readU32(FILE* file, uint32_t& value) {
        unsigned char bytes[4];
        if (!readExact(file, bytes, sizeof(bytes))) return false;
        value = static_cast<uint32_t>(bytes[0])
              | (static_cast<uint32_t>(bytes[1]) << 8)
              | (static_cast<uint32_t>(bytes[2]) << 16)
              | (static_cast<uint32_t>(bytes[3]) << 24);
        return true;
    }

    bool readU64(FILE* file, uint64_t& value) {
        unsigned char bytes[8];
        if (!readExact(file, bytes, sizeof(bytes))) return false;
        value = 0;
        for (int i = 0; i < 8; i++) {
            value |= static_cast<uint64_t>(bytes[i]) << (i * 8);
        }
        return true;
    }

    bool seekFile(FILE* file, uint64_t offset) {
#ifdef _WIN32
        return _fseeki64(file, static_cast<pack_offset_t>(offset), SEEK_SET) == 0;
#else
        return fseeko(file, static_cast<pack_offset_t>(offset), SEEK_SET) == 0;
#endif
    }

    bool getFileSize(FILE* file, uint64_t& size) {
#ifdef _WIN32
        if (_fseeki64(file, 0, SEEK_END) != 0) return false;
        const pack_offset_t end = _ftelli64(file);
#else
        if (fseeko(file, 0, SEEK_END) != 0) return false;
        const pack_offset_t end = ftello(file);
#endif
        if (end < 0) return false;

        size = static_cast<uint64_t>(end);
        return seekFile(file, 0);
    }

    bool loadPack(const std::string& filename) {
        FILE* file = System::instance().platformFopen(filename.c_str(), "rb");
        if (!file) return false;

        uint64_t packSize = 0;
        char magic[sizeof(PACK_MAGIC)];
        uint32_t fileCount = 0;
        bool ok = getFileSize(file, packSize)
            && packSize <= MAX_PACK_OFFSET
            && readExact(file, magic, sizeof(magic))
            && std::equal(std::begin(PACK_MAGIC), std::end(PACK_MAGIC), magic)
            && readU32(file, fileCount);

        std::map<std::string, PackEntry> entries;
        for (uint32_t i = 0; ok && i < fileCount; i++) {
            uint16_t pathLength = 0;
            if (!readU16(file, pathLength) || pathLength == 0) {
                ok = false;
                break;
            }

            std::string path(pathLength, '\0');
            if (!readExact(file, path.data(), pathLength)) {
                ok = false;
                break;
            }

            PackEntry entry;
            if (!readU64(file, entry.offset)
                || !readU64(file, entry.size)
                || !readU8(file, entry.key)
                || !readU32(file, entry.shift)) {
                ok = false;
                break;
            }

            // Bounds come from the file, so a corrupt one must not become an allocation
            if (entry.offset > packSize || entry.size > packSize - entry.offset
                || entry.size > UINT32_MAX) {
                ok = false;
                break;
            }

            entries[normalizePath(path)] = entry;
        }

        std::fclose(file);

        if (!ok) return false;

        state().filename = filename;
        state().entries = std::move(entries);
        return true;
    }

    void loadPackOnce() {
#ifndef DORIAX_EDITOR
        // Beside the executable on desktop, at the asset root inside an APK
        if (loadPack(PACK_FILENAME)) return;

        // Used as given: absolute in a macOS bundle, or wherever DORIAX_ASSET_PATH points
        const std::string assetRoot = System::instance().getAssetPath();
        if (!assetRoot.empty()) {
            loadPack(assetRoot + "/" + PACK_FILENAME);
        }
#endif
    }

    const PackEntry* findEntry(const std::string& path) {
        std::call_once(loadFlag, loadPackOnce);

        const PackState& pack = state();
        if (pack.entries.empty()) return nullptr;

        const std::string key = getPackKey(path);
        if (key.empty()) return nullptr;

        auto it = pack.entries.find(key);
        return it != pack.entries.end() ? &it->second : nullptr;
    }

    // Inverse of Exporter::obfuscate, which rotates right and then XORs
    void deobfuscate(std::vector<unsigned char>& data, uint8_t key, uint32_t shift) {
        if (data.empty()) return;

        for (unsigned char& byte : data) {
            byte ^= key;
        }

        shift %= data.size();
        if (shift > 0) {
            std::rotate(data.begin(), data.begin() + shift, data.end());
        }
    }

}

bool ResourcePack::contains(const std::string& path) {
    return findEntry(path) != nullptr;
}

bool ResourcePack::read(const std::string& path, std::vector<unsigned char>& outData) {
    const PackEntry* entry = findEntry(path);
    if (!entry) return false;

    FILE* file = System::instance().platformFopen(state().filename.c_str(), "rb");
    if (!file) return false;

    bool ok = seekFile(file, entry->offset);
    if (ok) {
        outData.resize(static_cast<size_t>(entry->size));
        ok = entry->size == 0 || readExact(file, outData.data(), static_cast<size_t>(entry->size));
    }
    std::fclose(file);

    if (!ok) {
        outData.clear();
        return false;
    }

    deobfuscate(outData, entry->key, entry->shift);
    return true;
}
