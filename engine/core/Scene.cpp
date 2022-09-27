#include "Scene.h"

#include "object/Camera.h"
#include "Engine.h"

#include "subsystem/RenderSystem.h"
#include "subsystem/MeshSystem.h"
#include "subsystem/UISystem.h"
#include "subsystem/ActionSystem.h"
#include "util/Color.h"

using namespace Supernova;

Scene::Scene(){
	registerComponent<MeshComponent>();
	registerComponent<ModelComponent>();
	registerComponent<BoneComponent>();
	registerComponent<SkyComponent>();
	registerComponent<UIComponent>();
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
	registerComponent<ParticlesAnimationComponent>();
	registerComponent<TextComponent>();
	registerComponent<ImageComponent>();
	registerComponent<ButtonComponent>();
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

	registerSystem<ActionSystem>();
	registerSystem<MeshSystem>();
	registerSystem<UISystem>();
	registerSystem<RenderSystem>();

	camera = NULL_ENTITY;

	mainScene = false;
	backgroundColor = Vector4(0.1, 0.1, 0.1, 1.0); //sRGB
	shadowsPCF = true;

	hasFog = false;

	hasSceneAmbientLight = false;
	Vector3 ambientLight = Vector3(1.0, 1.0, 1.0);
	float ambientFactor = 0.2;

	renderToTexture = false;
	framebufferWidth = 512;
	framebufferHeight = 512;
}

Scene::~Scene(){
	destroy();
}

void Scene::setCamera(Entity camera){
	if (findComponent<CameraComponent>(camera)){
		this->camera = camera;
	}else{
		Log::Error("Invalid camera entity: need CameraComponent");
	}
}

Entity Scene::getCamera() const{
	return camera;
}

Entity Scene::createDefaultCamera(){
	Entity defaultCamera = createEntity();
	addComponent<CameraComponent>(defaultCamera, {});
	addComponent<Transform>(defaultCamera, {});

	CameraComponent& camera = getComponent<CameraComponent>(defaultCamera);
	camera.type = CameraType::CAMERA_2D;

	return defaultCamera;
}

void Scene::setMainScene(bool mainScene){
	this->mainScene = mainScene;
}

bool Scene::isMainScene() const{
	return mainScene;
}

void Scene::setBackgroundColor(Vector4 color){
	this->backgroundColor = color;
}

void Scene::setBackgroundColor(float red, float green, float blue){
	setBackgroundColor(Vector4(red, green, blue, 1.0));
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

void Scene::setFogEnabled(bool hasFog){
	this->hasFog = hasFog;
}

bool Scene::isFogEnabled() const{
	return this->hasFog;
}

Fog& Scene::getFog(){
	return fog;
}

void Scene::setAmbientLight(float ambientFactor, Vector3 ambientLight){
	this->ambientFactor = ambientFactor;
	this->ambientLight = ambientLight;
	this->hasSceneAmbientLight = true;
}

void Scene::setAmbientLight(float ambientFactor){
	this->ambientFactor = ambientFactor;
	this->hasSceneAmbientLight = true;
}

void Scene::setAmbientLight(Vector3 ambientLight){
	this->ambientLight = ambientLight;
	this->hasSceneAmbientLight = true;
}

float Scene::getAmbientLightFactor() const{
	return this->ambientFactor;
}

Vector3 Scene::getAmbientLightColor() const{
	return this->ambientLight;
}

bool Scene::isSceneAmbientLightEnabled() const{
	return this->hasSceneAmbientLight;
}

void Scene::setSceneAmbientLightEnabled(bool hasSceneAmbientLight){
	this->hasSceneAmbientLight = hasSceneAmbientLight;
}

void Scene::setRenderToTexture(bool renderToTexture){
	this->renderToTexture = renderToTexture;
}

bool Scene::isRenderToTexture() const{
	return renderToTexture;
}

FramebufferRender& Scene::getFramebuffer(){
	return framebuffer;
}

void Scene::setFramebufferSize(int width, int height){
	this->framebufferWidth = width;
	this->framebufferHeight = height;

	if (renderToTexture){
		updateCameraSize();
	}
}

int Scene::getFramebufferWidth(){
	return framebufferWidth;
}

int Scene::getFramebufferHeight(){
	return framebufferHeight;
}

void Scene::updateCameraSize(){
	CameraComponent* cameraComp = findComponent<CameraComponent>(camera);
	if (cameraComp){
		Rect rect;
		if (!renderToTexture) {
			rect = Rect(0, 0, Engine::getCanvasWidth(), Engine::getCanvasHeight());
		}else{
			rect = Rect(0, 0, framebufferWidth, framebufferHeight);
		}

    	if (cameraComp->automatic){
        	float newLeft = rect.getX();
        	float newBottom = rect.getY();
        	float newRight = rect.getWidth();
        	float newTop = rect.getHeight();
        	float newAspect = rect.getWidth() / rect.getHeight();

        	if ((cameraComp->left != newLeft) || (cameraComp->bottom != newBottom) || (cameraComp->right != newRight) || (cameraComp->top != newTop) || (cameraComp->aspect != newAspect)){
            	cameraComp->left = newLeft;
            	cameraComp->bottom = newBottom;
            	cameraComp->right = newRight;
            	cameraComp->top = newTop;
            	cameraComp->aspect = newAspect;

            	cameraComp->needUpdate = true;
        	}
    	}
	}
}

void Scene::load(){
	if (camera == NULL_ENTITY){
		camera = createDefaultCamera();
	}

	for (auto const& pair : systems){
		pair.second->load();
	}
}

void Scene::destroy(){
	for (auto const& pair : systems){
		pair.second->destroy();
	}
	framebuffer.destroyFramebuffer();
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

Entity Scene::createEntity(){
    return entityManager.createEntity();
}

void Scene::destroyEntity(Entity entity){

	for (auto const& pair : systems){
		pair.second->entityDestroyed(entity);
	}

	componentManager.entityDestroyed(entity);
	
	entityManager.destroy(entity);
}

int32_t Scene::findBranchLastIndex(Entity entity){
	auto transforms = componentManager.getComponentArray<Transform>();

	size_t index = transforms->getIndex(entity);
	if (index < 0)
		return -1;

	size_t currentIndex = index + 1;
	std::vector<Entity> entityList;
	entityList.push_back(entity);

	bool found = false;
	while (!found){
		if (currentIndex < transforms.get()->size()){
			Transform& transform = componentManager.getComponentFromIndex<Transform>(currentIndex);
			//if not in list
			if (std::find(entityList.begin(), entityList.end(),transform.parent)==entityList.end()) {
				found = true;
			}else{
				entityList.push_back(transforms->getEntity(currentIndex));
				currentIndex++;
			}
		} else {
			found = true;
		}
	}

	currentIndex--;

	return currentIndex;
}

void Scene::addEntityChild(Entity parent, Entity child){
	Signature parentSignature = entityManager.getSignature(parent);
	Signature childSignature = entityManager.getSignature(child);
	
	Signature signature;
	signature.set(componentManager.getComponentType<Transform>(), true);

	if ( ((parentSignature & signature) == signature) && ((childSignature & signature) == signature) ){
		Transform& transformChild = componentManager.getComponent<Transform>(child);
		Transform& transformParent = componentManager.getComponent<Transform>(parent);

		auto transforms = componentManager.getComponentArray<Transform>();

		//----------DEBUG
		//Log::Debug("Add child - BEFORE");
		//for (int i = 0; i < transforms->size(); i++){
		//	auto transform = transforms->getComponentFromIndex(i);
		//	Log::Debug("Transform %i - Entity: %i - Parent: %i: %s", i, transforms->getEntity(i), transform.parent, transform.name.c_str());
		//}
		//----------DEBUG

		//size_t parentIndex = transforms->getIndex(parent);
		size_t childIndex = transforms->getIndex(child);

		transformChild.parent = parent;

		//find children of parent and child family
		size_t newIndex = findBranchLastIndex(parent) + 1;
		size_t lastChild = findBranchLastIndex(child);

		int length = lastChild - childIndex + 1;

		if (newIndex > childIndex){
			newIndex = newIndex - length;
		}

		if (childIndex != newIndex){
			if (length == 1){
				transforms->moveEntityToIndex(child, newIndex);
			}else{
				transforms->moveEntityRangeToIndex(child, transforms->getEntity(lastChild), newIndex);
			}
		}

		//----------DEBUG
		//Log::Debug("Add child - AFTER");
		//for (int i = 0; i < transforms->size(); i++){
		//	auto transform = transforms->getComponentFromIndex(i);
		//	Log::Debug("Transform %i - Entity: %i - Parent: %i: %s", i, transforms->getEntity(i), transform.parent, transform.name.c_str());
		//}
		//Log::Debug("\n");
		//----------DEBUG
	}

	sortComponentsByTransform(childSignature);
}

void Scene::sortComponentsByTransform(Signature entitySignature){
	// Mesh component
	if (entitySignature.test(getComponentType<MeshComponent>())){
		auto meshes = componentManager.getComponentArray<MeshComponent>();
		meshes->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Model component
	if (entitySignature.test(getComponentType<ModelComponent>())){
		auto models = componentManager.getComponentArray<ModelComponent>();
		models->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Bone component
	if (entitySignature.test(getComponentType<BoneComponent>())){
		auto bones = componentManager.getComponentArray<BoneComponent>();
		bones->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Polygon component
	if (entitySignature.test(getComponentType<PolygonComponent>())){
		auto polygons = componentManager.getComponentArray<PolygonComponent>();
		polygons->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// UI component
	if (entitySignature.test(getComponentType<UIComponent>())){
		auto ui = componentManager.getComponentArray<UIComponent>();
		ui->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}

	// Particles component
	if (entitySignature.test(getComponentType<ParticlesComponent>())){
		auto particles = componentManager.getComponentArray<ParticlesComponent>();
		particles->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
	}
}

void Scene::moveChildAux(Entity entity, bool increase, bool stopIfFound){
	Signature entitySignature = entityManager.getSignature(entity);
	
	Signature signature;
	signature.set(componentManager.getComponentType<Transform>(), true);

	if ((entitySignature & signature) == signature){
		auto transforms = componentManager.getComponentArray<Transform>();

		size_t entityIndex = transforms->getIndex(entity);
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

		if (nextIndex != entityIndex)
			transforms->moveEntityToIndex(entity, nextIndex);
	}

	sortComponentsByTransform(entitySignature);
}

void Scene::moveChildToFirst(Entity entity){
	moveChildAux(entity, true, false);
}

void Scene::moveChildUp(Entity entity){
	moveChildAux(entity, true, true);
}

void Scene::moveChildDown(Entity entity){
	moveChildAux(entity, false, true);
}

void Scene::moveChildToLast(Entity entity){
	moveChildAux(entity, false, false);
}