#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "SubSystem.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include <vector>
#include <unordered_map>

#include "component/MeshComponent.h"
#include "component/SkyComponent.h"
#include "component/SpriteComponent.h"
#include "component/Transform.h"
#include "component/LightComponent.h"
#include "subsystem/RenderSystem.h"

namespace Supernova{

	class Scene{
	private:

		Entity camera;
	
	    EntityManager entityManager;
	    ComponentManager componentManager;
		std::unordered_map<const char*, std::shared_ptr<SubSystem>> systems;

		void sortComponentsByTransform(Signature entitySignature);
		void moveChildAux(Entity entity, bool increase, bool stopIfFound);
		
	public:
	
		Scene();

		void load();
		void draw();
		void update(double dt);

		void setCamera(Entity camera);
		Entity getCamera();

		void updateCameraSize();

		size_t findBranchLastIndex(Entity entity);
	
		//Entity methods

	    Entity createEntity();
	
		void destroyEntity(Entity entity);

		void addEntityChild(Entity parent, Entity child);

		void moveChildToFirst(Entity entity);
		void moveChildUp(Entity entity);
		void moveChildDown(Entity entity);
		void moveChildToLast(Entity entity);
	
	    // Component methods

	    template<typename T>
	    void registerComponent(){
		    componentManager.registerComponent<T>();
	    }
	
	    template<typename T>
	    void addComponent(Entity entity, T component){
		    componentManager.addComponent<T>(entity, component);
		    auto signature = entityManager.getSignature(entity);
		    signature.set(componentManager.getComponentType<T>(), true);
		    entityManager.setSignature(entity, signature);
	    }
	
	    template<typename T>
	    void removeComponent(Entity entity){
		    componentManager.removeComponent<T>(entity);
		    auto signature = entityManager.getSignature(entity);
		    signature.set(componentManager.getComponentType<T>(), false);
		    entityManager.setSignature(entity, signature); 
	    }

		template<typename T>
	    T* findComponent(Entity entity) {
		    return componentManager.findComponent<T>(entity);
	    }
	
	    template<typename T>
	    T& getComponent(Entity entity) {
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
	    ComponentType getComponentType(){
		    return componentManager.getComponentType<T>();
	    }

		template<typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray() {
			return componentManager.getComponentArray<T>();
		}
	
		// System methods
	
		template<typename T>
		std::shared_ptr<T> registerSystem(){
			const char* typeName = typeid(T).name();
	
			assert(systems.find(typeName) == systems.end() && "Registering system more than once.");
	
			auto system = std::make_shared<T>(this);
			systems.insert({typeName, system});
			return system;
		}
	};

}

#endif //SCENE_H