// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Scene.h"
#include "math/Vector3.h"
#include "ecs/Entity.h"
#include "component/Transform.h"
#include "Catalog.h"
#include <functional>
#include <type_traits>


namespace doriax::editor{


    template<typename T, typename = void>
    struct is_iterable_container : std::false_type {};

    template<typename T>
    struct is_iterable_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()), typename T::value_type>> : std::true_type {};

    template<typename T, typename = void>
    struct is_equality_comparable : std::false_type {};

    template<typename T>
    struct is_equality_comparable<T, std::void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>> : std::true_type {};

    template<typename T, typename = void>
    struct safe_has_equality_operator : std::false_type {};

    template<typename T>
    struct safe_has_equality_operator<T, std::enable_if_t<!is_iterable_container<T>::value>>
        : is_equality_comparable<T> {};

    template<typename T>
    struct safe_has_equality_operator<T, std::enable_if_t<is_iterable_container<T>::value>>
        : is_equality_comparable<typename T::value_type> {};

    template<typename T>
    inline constexpr bool has_equality_operator_v = safe_has_equality_operator<T>::value;


    template<typename T>
    struct PropertyCmdValue{
        T oldValue;
        T newValue;
        uint32_t oldOverrideFields = 0; // submesh properties only, see getSubmeshOverrideMask
    };

    // Component types the Structure tree reads nothing from: it caches names, signature
    // icons, hierarchy and bundle/lock state, not component values. Opt-out list, so
    // anything unlisted stays structural.
    inline bool isNonStructuralProperty(ComponentType type, const std::string& propertyName){
        switch (type){
            // Geometry and sub-selection payloads, dragged continuously in the viewport.
            case ComponentType::InstancedMeshComponent:
            case ComponentType::TilemapComponent:
            case ComponentType::LinesComponent:
            case ComponentType::PointsComponent:
            case ComponentType::PolygonComponent:
            case ComponentType::MeshPolygonComponent:
            case ComponentType::Occluder2DComponent:
            case ComponentType::TranslateTracksComponent:
            case ComponentType::RotateTracksComponent:
            case ComponentType::ScaleTracksComponent:
            // Sizes written by the UI resize drag.
            case ComponentType::UILayoutComponent:
            case ComponentType::SpriteComponent:
            case ComponentType::TextComponent:
                return true;
            // Transform.parent drives tree order and lock state.
            case ComponentType::Transform:
                return propertyName == "position" || propertyName == "rotation" || propertyName == "scale";
            default:
                return false;
        }
    }

    template<typename T>
    class PropertyCmd: public Command{

    private:
        Project* project;
        uint32_t sceneId;
        ComponentType type;
        std::string propertyName;
        bool wasModified;
        std::function<void()> onValueChanged;

        std::map<Entity,PropertyCmdValue<T>> values;

    public:

        PropertyCmd(Project* project, uint32_t sceneId, Entity entity, ComponentType type, std::string propertyName, T newValue, std::function<void()> onValueChanged = nullptr){
            this->project = project;
            this->sceneId = sceneId;
            this->type = type;
            this->propertyName = propertyName;
            this->onValueChanged = onValueChanged;

            this->values[entity].newValue = newValue;
            this->wasModified = project->getScene(sceneId)->isModified;
        }

        bool execute() override{
            SceneProject* sceneProject = project->getScene(sceneId);
            if (!sceneProject){
                return false;
            }
            for (auto& [entity, value] : values){
                PropertyData prop = Catalog::findProperty(sceneProject->scene, entity, type, propertyName);
                T* valueRef = static_cast<T*>(prop.ref);
                // script properties are per entity, a selection may not share all of them
                if (!valueRef){
                    continue;
                }

                value.oldValue = T(*valueRef);
                *valueRef = value.newValue;

                // Read before the no-op check below: a command merging into this one adopts this
                // mask, and an entity already at the new value still carries its earlier edits.
                uint32_t propertyFields = 0;
                uint32_t* overrideMask = Catalog::getSubmeshOverrideMask(sceneProject->scene, entity, type, propertyName, propertyFields);
                if (overrideMask){
                    value.oldOverrideFields = *overrideMask;
                }

                if constexpr (has_equality_operator_v<T>) {
                    if (value.oldValue == value.newValue){
                        continue;
                    }
                }

                if (overrideMask){
                    *overrideMask |= propertyFields;
                }

                Catalog::updateEntity(sceneProject->scene, entity, prop.updateFlags);

                if (project->isEntityInBundle(sceneId, entity)){
                    project->bundlePropertyChanged(sceneId, entity, type, {propertyName});
                }
            }

            sceneProject->isModified = true;

            if (onValueChanged) {
                onValueChanged();
            }

            return true;
        }

        void undo() override{
            SceneProject* sceneProject = project->getScene(sceneId);
            if (!sceneProject){
                return;
            }
            for (auto const& [entity, value] : values){
                PropertyData prop = Catalog::findProperty(sceneProject->scene, entity, type, propertyName);
                T* valueRef = static_cast<T*>(prop.ref);
                if (!valueRef){
                    continue;
                }

                *valueRef = value.oldValue;

                // Mirrors execute: restored whether or not the value moved.
                uint32_t propertyFields = 0;
                if (uint32_t* overrideMask = Catalog::getSubmeshOverrideMask(sceneProject->scene, entity, type, propertyName, propertyFields)){
                    *overrideMask = value.oldOverrideFields;
                }

                if constexpr (has_equality_operator_v<T>) {
                    if (value.oldValue == value.newValue){
                        continue;
                    }
                }

                Catalog::updateEntity(sceneProject->scene, entity, prop.updateFlags);

                if (project->isEntityInBundle(sceneId, entity)){
                    project->bundlePropertyChanged(sceneId, entity, type, {propertyName});
                }
            }

            sceneProject->isModified = wasModified;

            if (onValueChanged) {
                onValueChanged();
            }
        }

        bool affectsStructure() const override{
            return !isNonStructuralProperty(type, propertyName);
        }

        bool mergeWith(editor::Command* otherCommand) override{
            PropertyCmd* otherCmd = dynamic_cast<PropertyCmd*>(otherCommand);
            if (otherCmd != nullptr){
                if (sceneId == otherCmd->sceneId && propertyName == otherCmd->propertyName){
                    for (auto const& [otherEntity, otherValue] : otherCmd->values){
                        if (values.find(otherEntity) != values.end()) {
                            // The older command holds the state a single undo has to restore.
                            values[otherEntity].oldValue = otherValue.oldValue;
                            values[otherEntity].oldOverrideFields = otherValue.oldOverrideFields;
                        }else{
                            values[otherEntity] = otherValue;
                        }
                    }
                    wasModified = wasModified && otherCmd->wasModified;
                    // Keep the most recent callback
                    if (otherCmd->onValueChanged) {
                        onValueChanged = otherCmd->onValueChanged;
                    }
                    return true;
                }
            }

            return false;
        }

    };

}
