#pragma once

#include <string>
#include <variant>

#include "ecs/Entity.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

// Macro with optional type parameter
#define SPROPERTY_1(DisplayName) \
public:

#define SPROPERTY_2(DisplayName, Type) \
public: /* @SPROPERTY_TYPE: Type */

// Macro overload selector
#define GET_SPROPERTY_MACRO(_1, _2, NAME, ...) NAME
#define SPROPERTY(...) GET_SPROPERTY_MACRO(__VA_ARGS__, SPROPERTY_2, SPROPERTY_1)(__VA_ARGS__)

namespace Supernova {

    class Scene;

    enum class ScriptPropertyType {
        Bool,
        Int,
        Float,
        String,
        Vector2,
        Vector3,
        Vector4,
        Color3,
        Color4,
        EntityPointer
    };

    struct EntityRef {
        Entity entity = NULL_ENTITY;
        Scene* scene = nullptr;
    };

    using ScriptPropertyValue = std::variant<
        std::monostate,  // For empty/uninitialized state
        bool,
        int, 
        float,
        std::string,
        Vector2,
        Vector3,
        Vector4,
        EntityRef
    >;

    struct ScriptProperty {
        std::string name;
        std::string displayName;
        ScriptPropertyType type;
        ScriptPropertyValue value;
        ScriptPropertyValue defaultValue;

        void* memberPtr = nullptr; // For editor use only

        // Optional: Store the actual type name for editor UI/debugging
        std::string ptrTypeName; // e.g., "Mesh*", "Object*", "EntityHandle*"

        // Helper template to get typed value
        template<typename T>
        T getValue() const {
            if (std::holds_alternative<T>(value)) {
                return std::get<T>(value);
            }
            return T{}; // Return default value if type mismatch
        }

        // Helper template to set typed value
        template<typename T>
        void setValue(const T& val) {
            value = val;
        }

        // Synchronize the stored value to the actual member variable
        void syncToMember() {
            if (!memberPtr) return;

            switch (type) {
                case ScriptPropertyType::Bool:
                    if (std::holds_alternative<bool>(value)) {
                        *static_cast<bool*>(memberPtr) = std::get<bool>(value);
                    }
                    break;
                case ScriptPropertyType::Int:
                    if (std::holds_alternative<int>(value)) {
                        *static_cast<int*>(memberPtr) = std::get<int>(value);
                    }
                    break;
                case ScriptPropertyType::Float:
                    if (std::holds_alternative<float>(value)) {
                        *static_cast<float*>(memberPtr) = std::get<float>(value);
                    }
                    break;
                case ScriptPropertyType::String:
                    if (std::holds_alternative<std::string>(value)) {
                        *static_cast<std::string*>(memberPtr) = std::get<std::string>(value);
                    }
                    break;
                case ScriptPropertyType::Vector2:
                    if (std::holds_alternative<Vector2>(value)) {
                        *static_cast<Vector2*>(memberPtr) = std::get<Vector2>(value);
                    }
                    break;
                case ScriptPropertyType::Vector3:
                case ScriptPropertyType::Color3:
                    if (std::holds_alternative<Vector3>(value)) {
                        *static_cast<Vector3*>(memberPtr) = std::get<Vector3>(value);
                    }
                    break;
                case ScriptPropertyType::Vector4:
                case ScriptPropertyType::Color4:
                    if (std::holds_alternative<Vector4>(value)) {
                        *static_cast<Vector4*>(memberPtr) = std::get<Vector4>(value);
                    }
                    break;
            }
        }

        // Synchronize from the actual member variable to the stored value
        void syncFromMember() {
            if (!memberPtr) return;

            switch (type) {
                case ScriptPropertyType::Bool:
                    value = *static_cast<bool*>(memberPtr);
                    break;
                case ScriptPropertyType::Int:
                    value = *static_cast<int*>(memberPtr);
                    break;
                case ScriptPropertyType::Float:
                    value = *static_cast<float*>(memberPtr);
                    break;
                case ScriptPropertyType::String:
                    value = *static_cast<std::string*>(memberPtr);
                    break;
                case ScriptPropertyType::Vector2:
                    value = *static_cast<Vector2*>(memberPtr);
                    break;
                case ScriptPropertyType::Vector3:
                case ScriptPropertyType::Color3:
                    value = *static_cast<Vector3*>(memberPtr);
                    break;
                case ScriptPropertyType::Vector4:
                case ScriptPropertyType::Color4:
                    value = *static_cast<Vector4*>(memberPtr);
                    break;
            }
        }
    };

}