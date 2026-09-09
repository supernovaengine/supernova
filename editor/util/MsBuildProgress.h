// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#ifdef _WIN32
#include <memory>
#endif
#include <string>

namespace doriax::editor {

    // Estimates Visual Studio/MSBuild compile progress from generated projects,
    // compiler tracking logs, and the source filenames streamed by MSBuild.
    class MsBuildProgress {
    public:
#ifdef _WIN32
        MsBuildProgress(const std::filesystem::path& buildDir, const std::string& configuration);
        ~MsBuildProgress();
#else
        MsBuildProgress(const std::filesystem::path&, const std::string&) {}
        bool consumeLine(const std::string&, float&) { return false; }
#endif

        MsBuildProgress(const MsBuildProgress&) = delete;
        MsBuildProgress& operator=(const MsBuildProgress&) = delete;

#ifdef _WIN32
        bool consumeLine(const std::string& line, float& fraction);

    private:
        class Impl;
        std::unique_ptr<Impl> impl;
#endif
    };

}
