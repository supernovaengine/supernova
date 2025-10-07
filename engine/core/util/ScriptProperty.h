#pragma once

#include <string>
#include <vector>
#include <functional>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

// Macros to make property declaration easier
#define SCRIPT_PROPERTY_BODY() \
private: \
    std::vector<Supernova::ScriptProperty> __scriptProperties; \
public: \
    const std::vector<Supernova::ScriptProperty>& getScriptProperties() const { return __scriptProperties; } \
    std::vector<Supernova::ScriptProperty>& getScriptPropertiesMutable() { return __scriptProperties; } \
private:

#define SPROPERTY(Type, DisplayName) \
public:

#define REGISTER_PROPERTY(VarName, DisplayName, DefaultValue) \
    do { \
        Supernova::ScriptProperty prop; \
        prop.name = #VarName; \
        prop.displayName = DisplayName; \
        prop.value = DefaultValue; \
        prop.defaultValue = DefaultValue; \
        prop.type = prop.value.type; \
        VarName = DefaultValue; \
        Supernova::ScriptPropertyType propType = prop.type; \
        prop.getter = [this, propType]() -> Supernova::ScriptPropertyValue { \
            Supernova::ScriptPropertyValue val; \
            val.type = propType; \
            val.set(this->VarName); \
            return val; \
        }; \
        prop.setter = [this](const Supernova::ScriptPropertyValue& val) { \
            this->VarName = val.get<decltype(this->VarName)>(); \
        }; \
        __scriptProperties.push_back(prop); \
    } while(0)


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
    std::string name;           // Internal name (variable name)
    std::string displayName;    // Display name in editor
    ScriptPropertyType type;
    ScriptPropertyValue value;
    ScriptPropertyValue defaultValue;

    // Getter and setter function pointers for accessing the actual C++ member variable
    std::function<void(const ScriptPropertyValue&)> setter;
    std::function<ScriptPropertyValue()> getter;

    ScriptProperty() : type(ScriptPropertyType::Bool) {}

    ScriptProperty(const std::string& name, const std::string& displayName, ScriptPropertyType type)
        : name(name), displayName(displayName), type(type) {
        value.type = type;
        defaultValue.type = type;
    }
};

} // namespace Supernova
