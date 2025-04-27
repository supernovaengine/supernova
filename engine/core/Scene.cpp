//
// (c) 2024 Eduardo Doria.
//

#include "Scene.h"

#include "object/Camera.h"
#include "Engine.h"

#include "object/Camera.h"

#include "subsystem/RenderSystem.h"
#include "subsystem/MeshSystem.h"
#include "subsystem/UISystem.h"
#include "subsystem/ActionSystem.h"
#include "subsystem/AudioSystem.h"
#include "subsystem/PhysicsSystem.h"
#include "util/Color.h"

using namespace Supernova;

Scene::Scene(){
	registerComponent<MeshComponent>();
	registerComponent<ModelComponent>();
	registerComponent<BoneComponent>();
	registerComponent<SkyComponent>();
	registerComponent<FogComponent>();
	registerComponent<UIContainerComponent>();
	registerComponent<UILayoutComponent>();
	registerComponent<SpriteComponent>();
	registerComponent<SpriteAnimationComponent>();
    registerComponent<Transform>();
	registerComponent<CameraComponent>();
	registerComponent<LightComponent>();
	registerComponent<ActionComponent>();
	registerComponent<TimedActionComponent>();
	registerComponent<PositionActionComponent>();
	registerComponent<RotationActionComponent>();
	registerComponent<ScaleActionComponent>();
	registerComponent<ColorActionComponent>();
	registerComponent<AlphaActionComponent>();
	registerComponent<ParticlesComponent>();
	registerComponent<PointsComponent>();
	registerComponent<LinesComponent>();
	registerComponent<TextComponent>();
	registerComponent<UIComponent>();
	registerComponent<ImageComponent>();
	registerComponent<ButtonComponent>();
	registerComponent<PanelComponent>();
	registerComponent<ScrollbarComponent>();
	registerComponent<TextEditComponent>();
	registerComponent<MeshPolygonComponent>();
	registerComponent<PolygonComponent>();
	registerComponent<AnimationComponent>();
	registerComponent<KeyframeTracksComponent>();
	registerComponent<MorphTracksComponent>();
	registerComponent<RotateTracksComponent>();
	registerComponent<TranslateTracksComponent>();
	registerComponent<ScaleTracksComponent>();
	registerComponent<TerrainComponent>();
	registerComponent<AudioComponent>();
	registerComponent<TilemapComponent>();
	registerComponent<Body2DComponent>();
	registerComponent<Joint2DComponent>();
	registerComponent<Body3DComponent>();
	registerComponent<Joint3DComponent>();
	registerComponent<InstancedMeshComponent>();

	registerSystem<ActionSystem>();
	registerSystem<MeshSystem>();
	registerSystem<UISystem>();
	registerSystem<RenderSystem>();
	registerSystem<PhysicsSystem>();
	registerSystem<AudioSystem>();

	camera = NULL_ENTITY;
	defaultCamera = NULL_ENTITY;

	backgroundColor = Vector4(0.0, 0.0, 0.0, 1.0); //sRGB
	shadowsPCF = true;

	hasSceneAmbientLight = false;
	ambientLight = Vector3(1.0, 1.0, 1.0);
	ambientIntensity = 0.1;

	enableUIEvents = true;
}

Scene::~Scene(){
	destroy();

	std::vector<Entity> entityList = entityManager.getEntityList();
	while(entityList.size() > 0){
		destroyEntity(entityList.front());
		// some entities can destroy other entities (ex: models)
		entityList = entityManager.getEntityList();
	}

}

void Scene::setCamera(Camera* camera){
	setCamera(camera->getEntity());
}

void Scene::setCamera(Entity camera){
	if (findComponent<CameraComponent>(camera)){
		this->camera = camera;
		if (defaultCamera != NULL_ENTITY){
			destroyEntity(defaultCamera);
			defaultCamera = NULL_ENTITY;
		}
	}else{
		Log::error("Invalid camera entity: need CameraComponent");
	}
}

Entity Scene::getCamera() const{
	return camera;
}

Entity Scene::createDefaultCamera(){
	defaultCamera = createEntity();
	addComponent<CameraComponent>(defaultCamera, {});
	addComponent<Transform>(defaultCamera, {});

	CameraComponent& camera = getComponent<CameraComponent>(defaultCamera);
	camera.type = CameraType::CAMERA_2D;
	camera.transparentSort = false;

	Transform& cameratransform = getComponent<Transform>(defaultCamera);
	cameratransform.position = Vector3(0.0, 0.0, 1.0);

	return defaultCamera;
}

void Scene::setBackgroundColor(Vector4 color){
	this->backgroundColor = color;
}

void Scene::setBackgroundColor(float red, float green, float blue){
	setBackgroundColor(Vector4(red, green, blue, 1.0));
}

void Scene::setBackgroundColor(float red, float green, float blue, float alpha){
	setBackgroundColor(Vector4(red, green, blue, alpha));
}

Vector4 Scene::getBackgroundColor() const{
	return backgroundColor;
}

void Scene::setShadowsPCF(bool shadowsPCF){
	this->shadowsPCF = shadowsPCF;
}

bool Scene::isShadowsPCF() const{
	return this->shadowsPCF;
}

void Scene::setAmbientLight(float ambientIntensity, Vector3 ambientLight){
	this->ambientIntensity = ambientIntensity;
	this->ambientLight = Color::sRGBToLinear(ambientLight);
	this->hasSceneAmbientLight = true;
}

void Scene::setAmbientLight(float ambientIntensity){
	this->ambientIntensity = ambientIntensity;
	this->hasSceneAmbientLight = true;
}

void Scene::setAmbientLight(Vector3 ambientLight){
	this->ambientLight = Color::sRGBToLinear(ambientLight);
	this->hasSceneAmbientLight = true;
}

float Scene::getAmbientLightIntensity() const{
	return this->ambientIntensity;
}

Vector3 Scene::getAmbientLightColor() const{
	return Color::linearTosRGB(this->ambientLight);
}

Vector3 Scene::getAmbientLightColorLinear() const{
	return this->ambientLight;
}

bool Scene::isSceneAmbientLightEnabled() const{
	return this->hasSceneAmbientLight;
}

void Scene::setSceneAmbientLightEnabled(bool hasSceneAmbientLight){
	this->hasSceneAmbientLight = hasSceneAmbientLight;
}

bool Scene::canReceiveUIEvents(){
	if (Engine::getLastScene() == this && this->enableUIEvents){
		return true;
	}
	return false;
}

bool Scene::isEnableUIEvents() const{
	return this->enableUIEvents;
}

void Scene::setEnableUIEvents(bool enableUIEvents){
	this->enableUIEvents = enableUIEvents;
}

void Scene::load(){
	if (camera == NULL_ENTITY){
		camera = createDefaultCamera();
	}
	
	for (auto const &pair: systems) {
		if (Engine::isViewLoaded()){
			pair.second->load();
		}
	}
}

void Scene::destroy(){
	for (auto const& pair : systems){
		pair.second->destroy();
	}
}

void Scene::draw(){
	for (auto const& pair : systems){
		pair.second->draw();
	}
}


void Scene::update(double dt){
	for (auto const& pair : systems){
		pair.second->update(dt);
	}
}

void Scene::updateSizeFromCamera(){
	getSystem<RenderSystem>()->updateCameraSize(getCamera());
}

Entity Scene::createEntityInternal(Entity entity){
	return entityManager.createEntityInternal(entity);
}

void Scene::setLastEntityInternal(Entity lastEntity){
	entityManager.setLastEntityInternal(lastEntity);
}

Entity Scene::getLastEntityInternal() const{
	return entityManager.getLastEntityInternal();
}

Entity Scene::createEntity(){
    return entityManager.createEntity();
}

bool Scene::isEntityCreated(Entity entity){
	return entityManager.isCreated(entity);
}

void Scene::destroyEntity(Entity entity){

	for (auto const& pair : systems){
		pair.second->entityDestroyed(entity);
	}

	componentManager.entityDestroyed(entity);
	
	entityManager.destroy(entity);
}

Entity Scene::findOldestParent(Entity entity){
	auto transforms = componentManager.getComponentArray<Transform>();

	size_t index = transforms->getIndex(entity);
	Entity lastParent = transforms->getComponentFromIndex(index).parent;

	for (int i = index - 1; i >= 0; i--){
		entity = transforms->getEntity(i);
		Transform& transform = transforms->getComponentFromIndex(i);
		if (entity == lastParent){
			lastParent = transform.parent;

			if (lastParent == NULL_ENTITY){
				return entity;
			}
		}
	}

	return entity;
}

bool Scene::isParentOf(Entity parent, Entity child){
	auto transforms = componentManager.getComponentArray<Transform>();

	size_t index = transforms->getIndex(child);
	Entity lastParent = transforms->getComponentFromIndex(index).parent;

	if (lastParent == parent){
		return true;
	}
	if (lastParent == NULL_ENTITY){
		return false;
	}

	for (int i = index - 1; i >= 0; i--){
		Entity entity = transforms->getEntity(i);
		Transform& transform = transforms->getComponentFromIndex(i);
		if (entity == lastParent){
			lastParent = transform.parent;

			if (lastParent == parent){
				return true;
			}
			if (lastParent == NULL_ENTITY){
				return false;
			}
		}
	}

	return false;
}

size_t Scene::findBranchLastIndex(Entity entity){
	auto transforms = componentManager.getComponentArray<Transform>();

	// will throw error if entity has not Transform
	size_t index = transforms->getIndex(entity);

	size_t currentIndex = index + 1;
	std::vector<Entity> entityList;
	entityList.push_back(entity);

	bool found = true;
	while (found){
		if (currentIndex < transforms.get()->size()){
			Transform& transform = componentManager.getComponentFromIndex<Transform>(currentIndex);
			//if not in list
			if (std::find(entityList.begin(), entityList.end(),transform.parent)==entityList.end()) {
				found = false;
			}else{
				entityList.push_back(transforms->getEntity(currentIndex));
				currentIndex++;
			}
		} else {
			found = false;
		}
	}

	currentIndex--;

	return currentIndex;
}

void Scene::addEntityChild(Entity parent, Entity child, bool changeTransform){
	//----------DEBUG
	//printf("Add child - BEFORE\n");
	//auto transforms = componentManager.getComponentArray<Transform>();
	//for (int i = 0; i < transforms->size(); i++){
	//	auto transform = transforms->getComponentFromIndex(i);
	//	printf("Transform %i - Entity: %i - Parent: %i: %s\n", i, transforms->getEntity(i), transform.parent, getEntityName(transforms->getEntity(i)).c_str());
	//}
	//----------DEBUG

	Signature childSignature = entityManager.getSignature(child);
	if (childSignature.test(getComponentId<Transform>())){
		Transform& transformChild = componentManager.getComponent<Transform>(child);

		transformChild.needUpdate = true;

		if (parent == NULL_ENTITY){
			if (transformChild.parent != NULL_ENTITY){
				size_t newIndex = findBranchLastIndex(findOldestParent(child)) + 1;
				moveChildToIndex(child, newIndex, true);

				Transform& transformChild = componentManager.getComponent<Transform>(child);

				transformChild.parent = NULL_ENTITY;
				if (changeTransform){
					// set local position to be the same of world position
					transformChild.modelMatrix.decompose(transformChild.position, transformChild.scale, transformChild.rotation);
				}
			}
		}else{
			Signature parentSignature = entityManager.getSignature(parent);
			if (parentSignature.test(getComponentId<Transform>())){
				size_t newIndex = findBranchLastIndex(parent) + 1;
				moveChildToIndex(child, newIndex, true);

				Transform& transformParent = componentManager.getComponent<Transform>(parent);
				Transform& transformChild = componentManager.getComponent<Transform>(child);

				if (transformChild.parent != parent) {
					transformChild.parent = parent;

					if (changeTransform){
						Matrix4 localMatrix = transformParent.modelMatrix.inverse() * transformChild.modelMatrix;
						localMatrix.decompose(transformChild.position, transformChild.scale, transformChild.rotation);
					}

				}
			}
		}
	}

	//----------DEBUG
	//printf("Add child - AFTER\n");
	//for (int i = 0; i < transforms->size(); i++){
	//	auto transform = transforms->getComponentFromIndex(i);
	//	printf("Transform %i - Entity: %i - Parent: %i: %s\n", i, transforms->getEntity(i), transform.parent, getEntityName(transforms->getEntity(i)).c_str());
	//}
	//printf("\n");
	//----------DEBUG
}

void Scene::sortComponentsByTransform(Signature entitySignature){
	// Mesh component
	if (entitySignature.test(getComponentId<MeshComponent>())){
		auto meshes = componentManager.getComponentArray<MeshComponent>();
		meshes->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// InstancedMesh component
	if (entitySignature.test(getComponentId<InstancedMeshComponent>())){
		auto instmeshes = componentManager.getComponentArray<InstancedMeshComponent>();
		instmeshes->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Model component
	if (entitySignature.test(getComponentId<ModelComponent>())){
		auto models = componentManager.getComponentArray<ModelComponent>();
		models->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Bone component
	if (entitySignature.test(getComponentId<BoneComponent>())){
		auto bones = componentManager.getComponentArray<BoneComponent>();
		bones->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Polygon component
	if (entitySignature.test(getComponentId<PolygonComponent>())){
		auto polygons = componentManager.getComponentArray<PolygonComponent>();
		polygons->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// UI layout component
	if (entitySignature.test(getComponentId<UILayoutComponent>())){
		auto layout = componentManager.getComponentArray<UILayoutComponent>();
		layout->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// UI component
	if (entitySignature.test(getComponentId<UIComponent>())){
		auto ui = componentManager.getComponentArray<UIComponent>();
		ui->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Points component
	if (entitySignature.test(getComponentId<PointsComponent>())){
		auto points = componentManager.getComponentArray<PointsComponent>();
		points->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Lines component
	if (entitySignature.test(getComponentId<PointsComponent>())){
		auto points = componentManager.getComponentArray<PointsComponent>();
		points->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Audio component
	if (entitySignature.test(getComponentId<AudioComponent>())){
		auto audios = componentManager.getComponentArray<AudioComponent>();
		audios->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}
}

void Scene::moveChildAux(Entity entity, bool increase, bool stopIfFound){
	Signature signature = entityManager.getSignature(entity);

	if (signature.test(getComponentId<Transform>())){
		auto transforms = componentManager.getComponentArray<Transform>();

		size_t entityIndex = transforms->getIndex(entity);
		//auto teste = transforms->getComponent(entity);
		Entity entityParent = transforms->getComponent(entity).parent;

		size_t nextIndex = entityIndex;
		if (increase){
			for (int i = (entityIndex+1); i < transforms->size(); i++){
				Transform& next = transforms->getComponentFromIndex(i);
				if (next.parent == entityParent){
					nextIndex = i;
					if (stopIfFound)
						break;
				}
			}
			nextIndex = findBranchLastIndex(transforms->getEntity(nextIndex));
		}else{
			for (int i = (entityIndex-1); i >= 0; i--){
				Transform& next = transforms->getComponentFromIndex(i);
				if (next.parent == entityParent){
					nextIndex = i;
					if (stopIfFound)
						break;
				}
			}
		}

		moveChildToIndex(entity, nextIndex, false);
	}
}

void Scene::moveChildToIndex(Entity entity, size_t index, bool adjustFinalPosition){
	Signature signature = entityManager.getSignature(entity);

	if (signature.test(getComponentId<Transform>())){
		auto transforms = componentManager.getComponentArray<Transform>();
		size_t entityIndex = transforms->getIndex(entity);

		if (index != entityIndex){
			size_t lastChildIndex = findBranchLastIndex(entity);
			size_t length = lastChildIndex - entityIndex + 1;

			if (adjustFinalPosition && (index > entityIndex)){
				index--;
			}

			if (length == 1){
				transforms->moveEntityToIndex(entity, index);
			}else{
				if (index > entityIndex){
					index = index - length + 1;
				}
				transforms->moveEntityRangeToIndex(entity, transforms->getEntity(lastChildIndex), index);
			}

			sortComponentsByTransform(entityManager.getSignature(entity));
		}
	}
}

void Scene::moveChildToTop(Entity entity){
	moveChildAux(entity, true, false);
}

void Scene::moveChildUp(Entity entity){
	moveChildAux(entity, true, true);
}

void Scene::moveChildDown(Entity entity){
	moveChildAux(entity, false, true);
}

void Scene::moveChildToBottom(Entity entity){
	moveChildAux(entity, false, false);
}

Signature Scene::getSignature(Entity entity) const{
	return entityManager.getSignature(entity);
}

void Scene::setEntityName(Entity entity, const std::string& name){
	return entityManager.setName(entity, name);
}

std::string Scene::getEntityName(Entity entity) const{
	return entityManager.getName(entity);
}