// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace doriax::editor {

    class ShaderHeaderBuilder {
    public:
        struct HeaderShader {
            std::string name;
            std::string encodedData;
        };

        static bool writeShaderHeader(const std::filesystem::path& outputPath, const std::string& suffix, const std::vector<HeaderShader>& shaders, std::string& err);

    private:
        static std::string buildShaderHeader(const std::string& suffix, std::vector<HeaderShader> shaders);
    };

} // namespace doriax::editor
