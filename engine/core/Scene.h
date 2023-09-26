//
// (c) 2023 Eduardo Doria.
//

#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "SubSystem.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include <vector>
#include <unordered_map>

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
#include "component/ParticlesAnimationComponent.h"
#include "component/TextComponent.h"
#include "component/UIComponent.h"
#include "component/ButtonComponent.h"
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

namespace Supernova{

	class Scene{
	private:

		Entity camera;
		Entity defaultCamera;

		Vector4 backgroundColor;
		bool shadowsPCF;

		bool hasSceneAmbientLight;
		Vector3 ambientLight;
		float ambientFactor;

		bool enableUIEvents;

	    EntityManager entityManager;
	    ComponentManager componentManager;
		std::vector<std::pair<const char*, std::shared_ptr<SubSystem>>> systems;

		Entity createDefaultCamera();
		void sortComponentsByTransform(Signature entitySignature);
		void moveChildAux(Entity entity, bool increase, bool stopIfFound);
		
	public:
	
		Scene();
		virtual ~Scene();

		void load();
		void destroy();
		void draw();
		void update(double dt);

		void updateSizeFromCamera();

		void setCamera(Entity camera);
		Entity getCamera() const;

		void setBackgroundColor(Vector4 color);
		void setBackgroundColor(float red, float green, float blue);
		Vector4 getBackgroundColor() const;

		void setShadowsPCF(bool shadowsPCF);
		bool isShadowsPCF() const;

		void setAmbientLight(float ambientFactor, Vector3 ambientLight);
		void setAmbientLight(float ambientFactor);
		void setAmbientLight(Vector3 ambientLight);

		float getAmbientLightFactor() const;
		Vector3 getAmbientLightColor() const;

		bool isSceneAmbientLightEnabled() const;
		void setSceneAmbientLightEnabled(bool hasSceneAmbientLight);

		bool canReceiveUIEvents();

		bool isEnableUIEvents() const;
		void setEnableUIEvents(bool enableUIEvents);

		int32_t findBranchLastIndex(Entity entity);
	
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

		Signature getSignature(Entity entity){
			return entityManager.getSignature(entity);
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
	
			auto it = std::find_if(systems.begin(), systems.end(), [&](const auto& pair) { return pair.first == typeName; });

			assert(it == systems.end() && "Registering system more than once");
	
			auto system = std::make_shared<T>(this);
			systems.push_back(std::make_pair(typeName, system));
			return system;
		}

		template<typename T>
		std::shared_ptr<T> getSystem(){
			const char* typeName = typeid(T).name();

			auto it = std::find_if(systems.begin(), systems.end(), [&](const auto& pair) { return pair.first == typeName; });

			assert(it != systems.end() && "System not found.");

			return std::dynamic_pointer_cast<T>(it->second);
		}
	};

}

#endif //SCENE_H