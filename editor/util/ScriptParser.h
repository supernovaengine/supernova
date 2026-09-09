// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "ScriptProperty.h"
#include <filesystem>
#include <optional>
#include <vector>
#include <string>

namespace doriax::editor {

    class ScriptParser {
    private:
        static doriax::ScriptPropertyType inferTypeFromCppType(const std::string& cppType, std::string& ptrTypeName);
        // nullopt falls back to the inferred C++ type
        static std::optional<doriax::ScriptPropertyType> parseExplicitType(const std::string& typeStr, const std::string& cppType);
        static std::string removeComments(const std::string& content);
        static std::optional<std::string> findScriptClassNameFromString(const std::string& content);

    public:
        // nullopt means that the file or named class could not be parsed.
        static std::optional<bool> inheritsScriptBase(const std::filesystem::path& scriptPath,
                                                      const std::string& className);
        static std::optional<bool> inheritsScriptBaseFromString(const std::string& content,
                                                                const std::string& className);
        // Ambiguous headers do not produce a class name.
        static std::optional<std::string> findScriptClassName(const std::filesystem::path& scriptPath);
        static std::vector<doriax::ScriptProperty> parseScriptProperties(const std::filesystem::path& scriptPath);
        static std::vector<doriax::ScriptProperty> parseScriptPropertiesFromString(const std::string& content, const std::string& sourceName = "memory");
    };

}
