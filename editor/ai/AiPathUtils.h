// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <string>

namespace doriax::editor::ai {

class PathUtils {
public:
    static bool isSafeRelativePath(const std::filesystem::path& path);
    static std::string slugify(const std::string& value, const std::string& fallback = "asset");
    static std::filesystem::path uniqueChildPath(const std::filesystem::path& directory,
                                                 const std::string& stem,
                                                 const std::string& extension);
};

} // namespace doriax::editor::ai
