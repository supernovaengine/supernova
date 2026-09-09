// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "ecs/Entity.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

// Macro with optional type parameter
#define DPROPERTY_1(DisplayName) \
public:

#define DPROPERTY_2(DisplayName, Type) \
public: /* @DPROPERTY_TYPE: Type */

// Macro overload selector
#define GET_DPROPERTY_MACRO(_1, _2, NAME, ...) NAME
#define DPROPERTY(...) GET_DPROPERTY_MACRO(__VA_ARGS__, DPROPERTY_2, DPROPERTY_1)(__VA_ARGS__)

namespace doriax {

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
        EntityReference
    };

    struct EntityReference {
        Entity entity = NULL_ENTITY;
        uint32_t sceneId = 0;
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
        EntityReference
    >;

    struct DORIAX_API ScriptProperty {
        std::string name;
        std::string displayName;
        ScriptPropertyType type = ScriptPropertyType::Bool;
        ScriptPropertyValue value;
        ScriptPropertyValue defaultValue;

        void* memberPtr = nullptr; // For editor use only
        void* ownedInstance = nullptr; // Wrapper created to fill an entity reference member
        int luaRef = 0; // For Lua script instance

        // Optional: Store the actual type name for editor UI/debugging
        std::string ptrTypeName; // e.g., "Mesh*", "Object*", "EntityHandle*"

        // Helper template to get typed value
        template<typename T>
        T getValue() const {
            if (std::holds_alternative<T>(value)) {
                return std::get<T>(value);
            }
            return T{};
        }

        // Helper template to get typed default value
        template<typename T>
        T getDefaultValue() const {
            if (std::holds_alternative<T>(defaultValue)) {
                return std::get<T>(defaultValue);
            }
            return T{};
        }

        // Helper template to set typed value
        template<typename T>
        void setValue(const T& val) {
            value = val;
        }

        // Value the caller can write through. A value left from the type the property
        // had before is replaced, std::get on it would throw
        template<typename T>
        T& valueRef() {
            if (!std::holds_alternative<T>(value)) {
                reportStaleValue();
                value = T{};
            }
            return std::get<T>(value);
        }

        template<typename T>
        T& defaultValueRef() {
            if (!std::holds_alternative<T>(defaultValue)) {
                reportStaleValue();
                defaultValue = T{};
            }
            return std::get<T>(defaultValue);
        }

        // Synchronize the stored value to the actual member variable
        void syncToMember();

        // Synchronize from the actual member variable to the stored value
        void syncFromMember();

    private:
        void reportStaleValue() const;
    };

}
