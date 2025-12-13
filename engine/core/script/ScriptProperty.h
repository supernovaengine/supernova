#pragma once

#include <cstdint>
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

    enum class EntityRefKind {
        None,
        LocalEntity,    // Resolved by sceneId + local entity id
        SharedEntity    // Resolved by shared group path + registry entity id
    };

    struct SUPERNOVA_API EntityLocator {
        EntityRefKind kind = EntityRefKind::None;
        Entity scopedEntity = NULL_ENTITY;

        // Scene-local addressing
        uint32_t sceneId = 0;

        // Shared-group addressing
        std::string sharedPath;
    };

    struct SUPERNOVA_API EntityRef {
        // Runtime resolution (valid during play/edit once resolved)
        Entity entity = NULL_ENTITY;
        Scene* scene = nullptr;

        // Editor locator (persistent, used to resolve 'entity' and 'scene')
        EntityLocator locator;
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

    struct SUPERNOVA_API ScriptProperty {
        std::string name;
        std::string displayName;
        ScriptPropertyType type;
        ScriptPropertyValue value;
        ScriptPropertyValue defaultValue;

        void* memberPtr = nullptr; // For editor use only
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

        // Helper template to set typed value
        template<typename T>
        void setValue(const T& val) {
            value = val;
        }

        // Synchronize the stored value to the actual member variable
        void syncToMember();

        // Synchronize from the actual member variable to the stored value
        void syncFromMember();
    };

}