// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ScriptParser.h"
#include "Out.h"
#include "FileUtils.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace doriax;

namespace {

bool isCppIdentifier(const std::string& value) {
    return !value.empty() &&
        (std::isalpha(static_cast<unsigned char>(value.front())) || value.front() == '_') &&
        std::all_of(value.begin() + 1, value.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '_';
        });
}

// Engine D_* constants, read from the headers so a new one needs no change here
const std::unordered_map<std::string, std::string>& engineConstants() {
    static const std::unordered_map<std::string, std::string> constants = []{
        std::unordered_map<std::string, std::string> parsed;
        const std::regex definePattern(R"(^\s*#define\s+(D_[A-Za-z0-9_]{0,64})\s+([^\s/]{1,64}))");
        for (const char* header : {"core/Input.h", "core/System.h"}) {
            std::ifstream file(editor::FileUtils::getEngineDir() / header);
            if (!file) continue;
            std::string line;
            while (std::getline(file, line)) {
                std::smatch match;
                if (std::regex_search(line, match, definePattern)) {
                    parsed.emplace(match[1].str(), match[2].str());
                }
            }
        }
        // Without the headers every D_* default silently reads as 0, so say why once
        if (parsed.empty()) {
            editor::Out::warning("No engine constants found in %s, constant defaults will read as 0",
                                 editor::FileUtils::getEngineDir().string().c_str());
        }
        return parsed;
    }();
    return constants;
}

// A number or an engine constant. Whitespace is already stripped by the caller.
std::optional<double> evaluateNumericDefault(const std::string& expression, int depth = 0) {
    if (expression.empty() || depth > 4) return std::nullopt;

    if (isCppIdentifier(expression)) {
        const auto& constants = engineConstants();
        const auto it = constants.find(expression);
        if (it == constants.end()) return std::nullopt;
        return evaluateNumericDefault(it->second, depth + 1); // D_KEY_LAST is D_KEY_MENU
    }

    // Literal suffixes, except on hex where f and F are digits
    const bool isHex = expression.compare(0, 2, "0x") == 0 || expression.compare(0, 2, "0X") == 0;
    std::string literal = expression;
    while (!literal.empty() && std::strchr(isHex ? "uUlL" : "fFuUlL", literal.back())) literal.pop_back();
    if (literal.empty()) return std::nullopt;

    const char* begin = literal.c_str();
    char* end = nullptr;
    const double value = isHex ? std::strtoll(begin, &end, 16) : std::strtod(begin, &end);
    if (end == begin || *end != '\0') return std::nullopt;

    return value;
}

// Unreadable components keep the zero the vector was built with
float vectorComponent(const std::string& expression) {
    const std::optional<double> parsed = evaluateNumericDefault(expression);
    return parsed ? static_cast<float>(*parsed) : 0.0f;
}

bool isEnumClass(const std::string& content, size_t classPosition) {
    size_t end = classPosition;
    while (end > 0 && std::isspace(static_cast<unsigned char>(content[end - 1]))) end--;
    size_t begin = end;
    while (begin > 0) {
        const unsigned char c = static_cast<unsigned char>(content[begin - 1]);
        if (!std::isalnum(c) && c != '_') break;
        begin--;
    }
    return content.compare(begin, end - begin, "enum") == 0;
}

std::optional<std::string> findClassBaseClause(const std::string& content,
                                               const std::string& className) {
    if (!isCppIdentifier(className)) return std::nullopt;

    const std::string pattern =
        R"(\b(?:class|struct)\s+(?:[A-Za-z_][A-Za-z0-9_]*\s+)*\b)" + className +
        R"(\b\s*(?:final\s*)?:\s*([^{};]*)\{)";
    const std::regex definitionPattern(pattern);
    for (std::sregex_iterator it(content.begin(), content.end(), definitionPattern), end;
            it != end; ++it) {
        if (!isEnumClass(content, static_cast<size_t>(it->position()))) {
            return (*it)[1].str();
        }
    }
    return std::nullopt;
}

} // namespace

ScriptPropertyType editor::ScriptParser::inferTypeFromCppType(const std::string& cppType, std::string& ptrTypeName) {
    // Remove const, &, and whitespace for comparison
    std::string cleanType = cppType;
    cleanType.erase(std::remove_if(cleanType.begin(), cleanType.end(), ::isspace), cleanType.end());

    // Remove const qualifier
    size_t constPos = cleanType.find("const");
    if (constPos != std::string::npos) {
        cleanType.erase(constPos, 5);
    }

    // Check if it's a pointer type (has *)
    bool isPointer = (cleanType.find('*') != std::string::npos);

    // Remove references and pointers for type matching
    std::string bareType = cleanType;
    bareType.erase(std::remove(bareType.begin(), bareType.end(), '&'), bareType.end());
    bareType.erase(std::remove(bareType.begin(), bareType.end(), '*'), bareType.end());

    // If it's a pointer type, store the type name without * and return Pointer
    if (isPointer) {
        ptrTypeName = bareType; // Store "Mesh", "Object", etc. (without *)
        return ScriptPropertyType::EntityReference;
    }

    // Map C++ types to ScriptPropertyType
    if (bareType == "bool") {
        return ScriptPropertyType::Bool;
    }

    if (bareType == "int" || bareType == "int32_t" || bareType == "uint32_t" ||
        bareType == "short" || bareType == "long") {
        return ScriptPropertyType::Int;
    }

    if (bareType == "float" || bareType == "double") {
        return ScriptPropertyType::Float;
    }

    if (bareType == "std::string" || bareType == "string") {
        return ScriptPropertyType::String;
    }

    if (bareType == "Vector2" || bareType == "doriax::Vector2") {
        return ScriptPropertyType::Vector2;
    }

    if (bareType == "Vector3" || bareType == "doriax::Vector3") {
        return ScriptPropertyType::Vector3;
    }

    if (bareType == "Vector4" || bareType == "doriax::Vector4") {
        return ScriptPropertyType::Vector4;
    }

    // Default to int if unknown
    Out::warning("Unknown C++ type '%s', defaulting to Int", cppType.c_str());
    return ScriptPropertyType::Int;
}

std::optional<ScriptPropertyType> editor::ScriptParser::parseExplicitType(const std::string& typeStr, const std::string& cppType) {
    std::string cleanType = typeStr;
    cleanType.erase(std::remove_if(cleanType.begin(), cleanType.end(), ::isspace), cleanType.end());

    if (cleanType == "Bool") return ScriptPropertyType::Bool;
    if (cleanType == "Int") return ScriptPropertyType::Int;
    if (cleanType == "Float") return ScriptPropertyType::Float;
    if (cleanType == "String") return ScriptPropertyType::String;
    if (cleanType == "Vector2") return ScriptPropertyType::Vector2;
    if (cleanType == "Vector3") return ScriptPropertyType::Vector3;
    if (cleanType == "Vector4") return ScriptPropertyType::Vector4;
    if (cleanType == "Color3") return ScriptPropertyType::Color3;
    if (cleanType == "Color4") return ScriptPropertyType::Color4;
    //if (cleanType == "Pointer") return ScriptPropertyType::Pointer;

    // Handle "Color" - infer based on C++ type
    if (cleanType == "Color") {
        // Remove whitespace from C++ type for comparison
        std::string cleanCppType = cppType;
        cleanCppType.erase(std::remove_if(cleanCppType.begin(), cleanCppType.end(), ::isspace), cleanCppType.end());

        // Remove const, &, * for bare type
        std::string bareCppType = cleanCppType;
        size_t constPos = bareCppType.find("const");
        if (constPos != std::string::npos) {
            bareCppType.erase(constPos, 5);
        }
        bareCppType.erase(std::remove(bareCppType.begin(), bareCppType.end(), '&'), bareCppType.end());
        bareCppType.erase(std::remove(bareCppType.begin(), bareCppType.end(), '*'), bareCppType.end());

        // Check if it's Vector4 or Vector3
        if (bareCppType == "Vector4" || bareCppType == "doriax::Vector4") {
            return ScriptPropertyType::Color4;
        } else {
            // Default to Color3 for Vector3 or any other type
            return ScriptPropertyType::Color3;
        }
    }

    Out::warning("Unknown explicit type '%s', will fall back to inferred type", typeStr.c_str());
    return std::nullopt;
}

std::string editor::ScriptParser::removeComments(const std::string& content) {
    std::string result;
    result.reserve(content.size());

    enum class State { Normal, InLineComment, InBlockComment, InString, InChar };
    State state = State::Normal;

    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];
        char next = (i + 1 < content.size()) ? content[i + 1] : '\0';

        switch (state) {
            case State::Normal:
                if (c == '/' && next == '/') {
                    state = State::InLineComment;
                    i++; // Skip next char
                } else if (c == '/' && next == '*') {
                    state = State::InBlockComment;
                    i++; // Skip next char
                } else if (c == '"') {
                    state = State::InString;
                    result += c;
                } else if (c == '\'') {
                    state = State::InChar;
                    result += c;
                } else {
                    result += c;
                }
                break;

            case State::InLineComment:
                if (c == '\n') {
                    state = State::Normal;
                    result += c; // Keep newline
                }
                break;

            case State::InBlockComment:
                if (c == '*' && next == '/') {
                    state = State::Normal;
                    i++; // Skip next char
                }
                break;

            case State::InString:
                result += c;
                if (c == '\\' && next != '\0') {
                    result += next;
                    i++; // Skip escaped char
                } else if (c == '"') {
                    state = State::Normal;
                }
                break;

            case State::InChar:
                result += c;
                if (c == '\\' && next != '\0') {
                    result += next;
                    i++; // Skip escaped char
                } else if (c == '\'') {
                    state = State::Normal;
                }
                break;
        }
    }

    return result;
}

std::optional<bool> editor::ScriptParser::inheritsScriptBase(const std::filesystem::path& scriptPath,
                                                             const std::string& className) {
    std::ifstream file(scriptPath);
    if (!file) return std::nullopt;
    const std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    return inheritsScriptBaseFromString(content, className);
}

std::optional<bool> editor::ScriptParser::inheritsScriptBaseFromString(const std::string& sourceContent,
                                                                       const std::string& className) {
    const std::string content = removeComments(sourceContent);
    const std::optional<std::string> baseClause = findClassBaseClause(content, className);
    if (!baseClause) return std::nullopt;

    static const std::regex scriptBasePattern(R"(\b(?:doriax::)?ScriptBase\b)");
    return std::regex_search(*baseClause, scriptBasePattern);
}

std::optional<std::string> editor::ScriptParser::findScriptClassName(
        const std::filesystem::path& scriptPath) {
    std::ifstream file(scriptPath);
    if (!file) return std::nullopt;
    const std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    return findScriptClassNameFromString(content);
}

std::optional<std::string> editor::ScriptParser::findScriptClassNameFromString(
        const std::string& sourceContent) {
    const std::string content = removeComments(sourceContent);
    static const std::regex definitionPattern(
        R"(\b(?:class|struct)\s+(?:[A-Za-z_][A-Za-z0-9_]*\s+)*?([A-Za-z_][A-Za-z0-9_]*)\s*(?:final\s*)?:\s*[^{};]*\{)");

    std::optional<std::string> candidate;
    for (std::sregex_iterator it(content.begin(), content.end(), definitionPattern), end;
            it != end; ++it) {
        if (!isEnumClass(content, static_cast<size_t>(it->position()))) {
            const std::string className = (*it)[1].str();
            if (candidate && *candidate != className) return std::nullopt;
            candidate = className;
        }
    }

    return candidate;
}

std::vector<ScriptProperty> editor::ScriptParser::parseScriptProperties(const std::filesystem::path& scriptPath) {
    std::vector<ScriptProperty> properties;

    if (!std::filesystem::exists(scriptPath)) {
        return properties;
    }

    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        Out::error("Failed to open script file for parsing: %s", scriptPath.string().c_str());
        return properties;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return parseScriptPropertiesFromString(content, scriptPath.string());
}

std::vector<ScriptProperty> editor::ScriptParser::parseScriptPropertiesFromString(const std::string& sourceContent, const std::string& sourceName) {
    std::vector<ScriptProperty> properties;
    std::string content = removeComments(sourceContent);

    // Updated pattern to capture optional type parameter and type annotation comment
    // Pattern: DPROPERTY("Display Name") or DPROPERTY("Display Name", Type) followed by Type varName = defaultValue;
    // Capped scans: std::regex recurses per character, an unterminated one overflows the stack
    std::regex propertyRegex(
        "DPROPERTY\\s*\\(\\s*"                    // DPROPERTY(
        "\"([^\"]{1,128})\"\\s*"                   // "Display Name"
        "(?:,\\s*([\\w]+))?\\s*"                   // optional , Type
        "\\)\\s*"                                  // )
        "(?:/\\*[^*]*@DPROPERTY_TYPE:\\s*([\\w]+)[^*]*\\*/\\s*)?" // optional /* @DPROPERTY_TYPE: Type */
        "([\\w:]+(?:\\s*<[^>]+>)?)"               // C++ Type (with templates)
        "([\\s*]+)"                                // separator: whitespace and/or '*' (either side of the name)
        "(\\w+)\\s*"                               // varName
        "(?:=\\s*([^;]{1,256}?))?\\s*;"           // optional = defaultValue
    );

    std::sregex_iterator it(content.begin(), content.end(), propertyRegex);
    std::sregex_iterator end;

    for (; it != end; ++it) {
        std::smatch match = *it;
        size_t position = match.position();

        std::string displayName = match[1].str();
        std::string explicitType = match[2].str();        // From DPROPERTY(..., Type)
        std::string typeAnnotation = match[3].str();      // From /* @DPROPERTY_TYPE: Type */
        // Combine the type name (group 4) with the pointer/reference markers (group 5)
        // so that '*' is retained regardless of whether it was written next to the
        // type ("Camera* c") or next to the variable ("Camera *c").
        std::string cppType = match[4].str() + match[5].str();
        std::string varName = match[6].str();
        std::string defaultValueStr = match[7].str();     // May be empty

        // Remove whitespace from type (but keep * for pointers)
        std::string cleanCppType;
        for (char c : cppType) {
            if (!std::isspace(c)) {
                cleanCppType += c;
            }
        }
        cppType = cleanCppType;

        // Remove whitespace from default value if present
        if (!defaultValueStr.empty()) {
            defaultValueStr.erase(std::remove_if(defaultValueStr.begin(), defaultValueStr.end(), ::isspace), defaultValueStr.end());
        }

        // Determine the property type
        ScriptPropertyType type;
        std::string ptrTypeName;

        // Priority: explicit type parameter > type annotation > inferred from C++ type
        std::optional<ScriptPropertyType> declaredType;
        if (!explicitType.empty()) {
            declaredType = parseExplicitType(explicitType, cppType);
        } else if (!typeAnnotation.empty()) {
            declaredType = parseExplicitType(typeAnnotation, cppType);
        }
        if (declaredType) {
            type = *declaredType;
        } else {
            type = inferTypeFromCppType(cppType, ptrTypeName);
        }

        // nullptr only means something on a pointer, stoi/stof would just throw on it
        if ((defaultValueStr == "nullptr" || defaultValueStr == "NULL") &&
            type != ScriptPropertyType::EntityReference) {
            Out::warning("Property '%s' in %s is not a pointer, ignoring its %s default value",
                         varName.c_str(), sourceName.c_str(), defaultValueStr.c_str());
            defaultValueStr.clear();
        }

        ScriptProperty prop;
        prop.name = varName;
        prop.displayName = displayName;
        prop.type = type;
        prop.ptrTypeName = ptrTypeName; // Will be empty for non-pointer types

        // Parse and set default value based on type
        try {
            switch (type) {
                case ScriptPropertyType::Bool: {
                    bool val = false; // default value
                    if (!defaultValueStr.empty()) {
                        val = (defaultValueStr == "true" || defaultValueStr == "1");
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::Int: {
                    int val = 0; // default value
                    if (!defaultValueStr.empty()) {
                        if (const auto parsed = evaluateNumericDefault(defaultValueStr)) {
                            val = static_cast<int>(*parsed);
                        } else {
                            Out::warning("Default value '%s' of property '%s' in %s is not a number, using 0",
                                         defaultValueStr.c_str(), varName.c_str(), sourceName.c_str());
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::Float: {
                    float val = 0.0f; // default value
                    if (!defaultValueStr.empty()) {
                        if (const auto parsed = evaluateNumericDefault(defaultValueStr)) {
                            val = static_cast<float>(*parsed);
                        } else {
                            Out::warning("Default value '%s' of property '%s' in %s is not a number, using 0",
                                         defaultValueStr.c_str(), varName.c_str(), sourceName.c_str());
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::String: {
                    std::string val = ""; // default value
                    if (!defaultValueStr.empty()) {
                        // Remove quotes if present
                        val = defaultValueStr;
                        if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                            val = val.substr(1, val.size() - 2);
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::Vector2: {
                    Vector2 val = Vector2(); // default value
                    if (!defaultValueStr.empty()) {
                        // Parse Vector2(x, y) or doriax::Vector2(x, y)
                        std::regex vecRegex("(?:doriax::)?Vector2\\(([^,]+),([^)]+)\\)");
                        std::smatch vecMatch;
                        if (std::regex_search(defaultValueStr, vecMatch, vecRegex)) {
                            val = Vector2(vectorComponent(vecMatch[1].str()),
                                          vectorComponent(vecMatch[2].str()));
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::Vector3:
                case ScriptPropertyType::Color3: {
                    Vector3 val = Vector3(); // default value
                    if (!defaultValueStr.empty()) {
                        // Parse Vector3(x, y, z) or doriax::Vector3(x, y, z)
                        std::regex vecRegex("(?:doriax::)?Vector3\\(([^,]+),([^,]+),([^)]+)\\)");
                        std::smatch vecMatch;
                        if (std::regex_search(defaultValueStr, vecMatch, vecRegex)) {
                            val = Vector3(vectorComponent(vecMatch[1].str()),
                                          vectorComponent(vecMatch[2].str()),
                                          vectorComponent(vecMatch[3].str()));
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::Vector4:
                case ScriptPropertyType::Color4: {
                    Vector4 val = Vector4(); // default value
                    if (!defaultValueStr.empty()) {
                        // Parse Vector4(x, y, z, w) or doriax::Vector4(x, y, z, w)
                        std::regex vecRegex("(?:doriax::)?Vector4\\(([^,]+),([^,]+),([^,]+),([^)]+)\\)");
                        std::smatch vecMatch;
                        if (std::regex_search(defaultValueStr, vecMatch, vecRegex)) {
                            val = Vector4(vectorComponent(vecMatch[1].str()),
                                          vectorComponent(vecMatch[2].str()),
                                          vectorComponent(vecMatch[3].str()),
                                          vectorComponent(vecMatch[4].str()));
                        }
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }

                case ScriptPropertyType::EntityReference: {
                    // Pointers default to null entity
                    EntityReference val{NULL_ENTITY, 0};
                    if (!defaultValueStr.empty() && defaultValueStr != "nullptr" && defaultValueStr != "NULL") {
                        Out::warning("Non-null entity pointer default values are not supported, using null entity for '%s'", varName.c_str());
                    }
                    prop.value = val;
                    prop.defaultValue = val;
                    break;
                }
            }
        } catch (const std::exception& e) {
            size_t lineNum = std::count(content.begin(), content.begin() + position, '\n') + 1;
            Out::warning("Failed to parse property at line %zu: %s", lineNum, e.what());
            // Initialize with default values on parse error
            switch (type) {
                case ScriptPropertyType::Bool:
                    prop.value = false;
                    prop.defaultValue = false;
                    break;
                case ScriptPropertyType::Int:
                    prop.value = 0;
                    prop.defaultValue = 0;
                    break;
                case ScriptPropertyType::Float:
                    prop.value = 0.0f;
                    prop.defaultValue = 0.0f;
                    break;
                case ScriptPropertyType::String:
                    prop.value = std::string("");
                    prop.defaultValue = std::string("");
                    break;
                case ScriptPropertyType::Vector2:
                    prop.value = Vector2();
                    prop.defaultValue = Vector2();
                    break;
                case ScriptPropertyType::Vector3:
                case ScriptPropertyType::Color3:
                    prop.value = Vector3();
                    prop.defaultValue = Vector3();
                    break;
                case ScriptPropertyType::Vector4:
                case ScriptPropertyType::Color4:
                    prop.value = Vector4();
                    prop.defaultValue = Vector4();
                    break;
                case ScriptPropertyType::EntityReference:
                    prop.value = EntityReference{NULL_ENTITY, 0};
                    prop.defaultValue = EntityReference{NULL_ENTITY, 0};
                    break;
            }
        }

        properties.push_back(prop);
    }

    std::unordered_set<std::string> seenNames;
    for (const auto& prop : properties) {
        if (!seenNames.insert(prop.name).second) {
            Out::warning("Duplicate property name '%s' in %s", 
                         prop.name.c_str(), sourceName.c_str());
        }
    }

    return properties;
}
