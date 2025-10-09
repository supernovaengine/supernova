#pragma once

#include <string>
#include <variant>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#define SPROPERTY(DisplayName) \
public:

namespace Supernova {

enum class ScriptPropertyType {
    Bool,
    Int,
    Float,
    String,
    Vector2,
    Vector3,
    Vector4,
    Color3,  // Vector3 but edited as color
    Color4   // Vector4 but edited as color
};

struct ScriptPropertyValue {
    ScriptPropertyType type;

    // POD values stored in union for efficiency
    union {
        bool boolValue;
        int intValue;
        float floatValue;
    };

    // Complex types stored separately
    std::string stringValue;
    Vector2 vector2Value;
    Vector3 vector3Value;
    Vector4 vector4Value;

    ScriptPropertyValue() : type(ScriptPropertyType::Bool), boolValue(false) {}
    ScriptPropertyValue(bool val) : type(ScriptPropertyType::Bool), boolValue(val) {}
    ScriptPropertyValue(int val) : type(ScriptPropertyType::Int), intValue(val) {}
    ScriptPropertyValue(float val) : type(ScriptPropertyType::Float), floatValue(val) {}
    ScriptPropertyValue(const std::string& val) : type(ScriptPropertyType::String), stringValue(val) {}
    ScriptPropertyValue(const Vector2& val) : type(ScriptPropertyType::Vector2), vector2Value(val) {}
    ScriptPropertyValue(const Vector3& val) : type(ScriptPropertyType::Vector3), vector3Value(val) {}
    ScriptPropertyValue(const Vector4& val) : type(ScriptPropertyType::Vector4), vector4Value(val) {}

    // Getter templates
    template<typename T> T get() const;

    // Setter templates
    template<typename T> void set(const T& value);
};

// Template specializations for get
template<> inline bool ScriptPropertyValue::get<bool>() const { return boolValue; }
template<> inline int ScriptPropertyValue::get<int>() const { return intValue; }
template<> inline float ScriptPropertyValue::get<float>() const { return floatValue; }
template<> inline std::string ScriptPropertyValue::get<std::string>() const { return stringValue; }
template<> inline Vector2 ScriptPropertyValue::get<Vector2>() const { return vector2Value; }
template<> inline Vector3 ScriptPropertyValue::get<Vector3>() const { return vector3Value; }
template<> inline Vector4 ScriptPropertyValue::get<Vector4>() const { return vector4Value; }

// Template specializations for set
template<> inline void ScriptPropertyValue::set<bool>(const bool& value) { boolValue = value; }
template<> inline void ScriptPropertyValue::set<int>(const int& value) { intValue = value; }
template<> inline void ScriptPropertyValue::set<float>(const float& value) { floatValue = value; }
template<> inline void ScriptPropertyValue::set<std::string>(const std::string& value) { stringValue = value; }
template<> inline void ScriptPropertyValue::set<Vector2>(const Vector2& value) { vector2Value = value; }
template<> inline void ScriptPropertyValue::set<Vector3>(const Vector3& value) { vector3Value = value; }
template<> inline void ScriptPropertyValue::set<Vector4>(const Vector4& value) { vector4Value = value; }

struct ScriptProperty {
    std::string name;
    std::string displayName;
    ScriptPropertyType type;
    ScriptPropertyValue value;
    ScriptPropertyValue defaultValue;

    void* memberPtr = nullptr; // For editor use only

    // Synchronize the stored value to the actual member variable
    void syncToMember() {
        if (!memberPtr) return;

        switch (type) {
            case ScriptPropertyType::Bool:
                *static_cast<bool*>(memberPtr) = value.get<bool>();
                break;
            case ScriptPropertyType::Int:
                *static_cast<int*>(memberPtr) = value.get<int>();
                break;
            case ScriptPropertyType::Float:
                *static_cast<float*>(memberPtr) = value.get<float>();
                break;
            case ScriptPropertyType::String:
                *static_cast<std::string*>(memberPtr) = value.get<std::string>();
                break;
            case ScriptPropertyType::Vector2:
                *static_cast<Vector2*>(memberPtr) = value.get<Vector2>();
                break;
            case ScriptPropertyType::Vector3:
            case ScriptPropertyType::Color3:
                *static_cast<Vector3*>(memberPtr) = value.get<Vector3>();
                break;
            case ScriptPropertyType::Vector4:
            case ScriptPropertyType::Color4:
                *static_cast<Vector4*>(memberPtr) = value.get<Vector4>();
                break;
        }
    }

    // Synchronize from the actual member variable to the stored value
    void syncFromMember() {
        if (!memberPtr) return;

        switch (type) {
            case ScriptPropertyType::Bool:
                value.set<bool>(*static_cast<bool*>(memberPtr));
                break;
            case ScriptPropertyType::Int:
                value.set<int>(*static_cast<int*>(memberPtr));
                break;
            case ScriptPropertyType::Float:
                value.set<float>(*static_cast<float*>(memberPtr));
                break;
            case ScriptPropertyType::String:
                value.set<std::string>(*static_cast<std::string*>(memberPtr));
                break;
            case ScriptPropertyType::Vector2:
                value.set<Vector2>(*static_cast<Vector2*>(memberPtr));
                break;
            case ScriptPropertyType::Vector3:
            case ScriptPropertyType::Color3:
                value.set<Vector3>(*static_cast<Vector3*>(memberPtr));
                break;
            case ScriptPropertyType::Vector4:
            case ScriptPropertyType::Color4:
                value.set<Vector4>(*static_cast<Vector4*>(memberPtr));
                break;
        }
    }
};

}