#ifndef ENTITYCONTAINER_H
#define ENTITYCONTAINER_H

#include "Entity.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Signature.h"
#include <memory>

#include "component/MeshComponent.h"
#include "component/ModelComponent.h"
#include "component/BoneComponent.h"
#include "component/SkyComponent.h"
#include "component/FogComponent.h"
#include "component/ImageComponent.h"
#include "component/UILayoutComponent.h"
#include "component/UIContainerComponent.h"
#include "component/SpriteComponent.h"
#include "component/SpriteAnimationComponent.h"
#include "component/Transform.h"
#include "component/LightComponent.h"
#include "component/ActionComponent.h"
#include "component/TimedActionComponent.h"
#include "component/PositionActionComponent.h"
#include "component/RotationActionComponent.h"
#include "component/ScaleActionComponent.h"
#include "component/ColorActionComponent.h"
#include "component/AlphaActionComponent.h"
#include "component/ParticlesComponent.h"
#include "component/PointsComponent.h"
#include "component/LinesComponent.h"
#include "component/TextComponent.h"
#include "component/UIComponent.h"
#include "component/ButtonComponent.h"
#include "component/PanelComponent.h"
#include "component/ScrollbarComponent.h"
#include "component/TextEditComponent.h"
#include "component/MeshPolygonComponent.h"
#include "component/PolygonComponent.h"
#include "component/AnimationComponent.h"
#include "component/KeyframeTracksComponent.h"
#include "component/MorphTracksComponent.h"
#include "component/RotateTracksComponent.h"
#include "component/TranslateTracksComponent.h"
#include "component/ScaleTracksComponent.h"
#include "component/TerrainComponent.h"
#include "component/AudioComponent.h"
#include "component/TilemapComponent.h"
#include "component/Body2DComponent.h"
#include "component/Joint2DComponent.h"
#include "component/Body3DComponent.h"
#include "component/Joint3DComponent.h"
#include "component/InstancedMeshComponent.h"

namespace Supernova {

    class SUPERNOVA_API EntityContainer {
    private:
        EntityManager entityManager;
        ComponentManager componentManager;
        bool componentsRegistered = false;

        void sortComponentsByTransform(Signature entitySignature);
		void moveChildAux(Entity entity, bool increase, bool stopIfFound);

    protected:
        virtual void onComponentAdded(Entity entity, ComponentId componentId) {}
        virtual void onComponentRemoved(Entity entity, ComponentId componentId) {}
        virtual void onEntityDestroyed(Entity entity, Signature signature) {}

    public:
        EntityContainer();
        virtual ~EntityContainer();

        //Entity methods

        bool recreateEntity(Entity entity); // for internal editor use only

        Entity createEntity();

        bool isEntityCreated(Entity entity) const;

        void destroyEntity(Entity entity);

        void addEntityChild(Entity parent, Entity child, bool changeTransform);

        void moveChildToIndex(Entity entity, size_t index, bool adjustFinalPosition); // for internal/editor use

        void moveChildToTop(Entity entity);
        void moveChildUp(Entity entity);
        void moveChildDown(Entity entity);
        void moveChildToBottom(Entity entity);

        Signature getSignature(Entity entity) const;

        void setEntityName(Entity entity, const std::string& name);
        std::string getEntityName(Entity entity) const;

        Entity findOldestParent(Entity entity);
        bool isParentOf(Entity parent, Entity child);
        size_t findBranchLastIndex(Entity entity);

        // Component methods

        template<typename T>
        void registerComponent(){
            componentManager.registerComponent<T>();
        }

        template<typename T>
        void addComponent(Entity entity, T component){
            componentManager.addComponent<T>(entity, component);
            auto signature = entityManager.getSignature(entity);
            signature.set(componentManager.getComponentId<T>(), true);
            entityManager.setSignature(entity, signature);

            onComponentAdded(entity, componentManager.getComponentId<T>());
        }

        template<typename T>
        void removeComponent(Entity entity){
            onComponentRemoved(entity, componentManager.getComponentId<T>());

            componentManager.removeComponent<T>(entity);
            auto signature = entityManager.getSignature(entity);
            signature.set(componentManager.getComponentId<T>(), false);
            entityManager.setSignature(entity, signature); 
        }

        template<typename T>
        T* findComponent(Entity entity) {
            return componentManager.findComponent<T>(entity);
        }

        template<typename T>
        T& getComponent(Entity entity) const {
            return componentManager.getComponent<T>(entity);
        }

        template<typename T>
        T* findComponentFromIndex(size_t index) {
            return componentManager.findComponentFromIndex<T>(index);
        }

        template<typename T>
        T& getComponentFromIndex(size_t index) {
            return componentManager.getComponentFromIndex<T>(index);
        }

        template<typename T>
        ComponentId getComponentId() const{
            return componentManager.getComponentId<T>();
        }

        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() const{
            return componentManager.getComponentArray<T>();
        }
    };

} // namespace Supernova

#endif // ENTITYCONTAINER_H