//
// (c) 2024 Eduardo Doria.
//

#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "SubSystem.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Export.h"
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

namespace Supernova{

	class Camera;

	enum class LightState {
		OFF,
		ON,
		AUTO
	};

	class SUPERNOVA_API Scene{
	private:

		Entity camera;
		Entity defaultCamera;

		Vector4 backgroundColor;
		bool shadowsPCF;

		LightState lightState;
		Vector3 globalIllumColor;
		float globalIllumIntensity;

		bool enableUIEvents;

	    EntityManager entityManager;
	    ComponentManager componentManager;
		std::vector<std::pair<std::string, std::shared_ptr<SubSystem>>> systems;

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

		void setCamera(Camera* camera);
		void setCamera(Entity camera);
		Entity getCamera() const;

		void setBackgroundColor(Vector4 color);
		void setBackgroundColor(float red, float green, float blue);
		void setBackgroundColor(float red, float green, float blue, float alpha);
		Vector4 getBackgroundColor() const;

		void setShadowsPCF(bool shadowsPCF);
		bool isShadowsPCF() const;

		void setLightState(LightState state);
		LightState getLightState() const;

		void setGlobalIllumination(float intensity, Vector3 color);
		void setGlobalIllumination(float intensity);
		void setGlobalIllumination(Vector3 color);

		float getGlobalIlluminationIntensity() const;
		Vector3 getGlobalIlluminationColor() const;
		Vector3 getGlobalIlluminationColorLinear() const;

		bool canReceiveUIEvents();

		bool isEnableUIEvents() const;
		void setEnableUIEvents(bool enableUIEvents);

		Entity findOldestParent(Entity entity);
		bool isParentOf(Entity parent, Entity child);
		size_t findBranchLastIndex(Entity entity);

		//Entity methods

		bool recreateEntity(Entity entity); // for internal editor use only

		Entity createEntity();

		bool isEntityCreated(Entity entity);

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

			ComponentId componentId = componentManager.getComponentId<T>();
			for (auto const& pair : systems){
				pair.second->onComponentAdded(entity, componentId);
			}
		}

		template<typename T>
		void removeComponent(Entity entity){
			ComponentId componentId = componentManager.getComponentId<T>();
			for (auto const& pair : systems){
				pair.second->onComponentRemoved(entity, componentId);
			}

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