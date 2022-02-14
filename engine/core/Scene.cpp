#include "Scene.h"

#include "object/Camera.h"
#include "Engine.h"

#include "subsystem/RenderSystem.h"
#include "subsystem/MeshSystem.h"
#include "subsystem/UISystem.h"
#include "subsystem/ActionSystem.h"

using namespace Supernova;

Scene::Scene(){
	registerComponent<MeshComponent>();
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

	registerSystem<RenderSystem>();
	registerSystem<MeshSystem>();
	registerSystem<UISystem>();
	registerSystem<ActionSystem>();

	camera = NULL_ENTITY;

	mainScene = false;
	color = Vector4(0.1, 0.1, 0.1, 1.0);
	shadowsPCF = true;

	renderToTexture = false;
	framebufferWidth = 512;
	framebufferHeight = 512;
}

Scene::~Scene(){
	framebuffer.destroyFramebuffer();
}

void Scene::setCamera(Entity camera){
	if (findComponent<CameraComponent>(camera)){
		this->camera = camera;
	}else{
		Log::Error("Invalid camera entity: need CameraComponent");
	}
}

Entity Scene::getCamera(){
	return camera;
}

Entity Scene::createDefaultCamera(){
	Entity defaultCamera = createEntity();
	addComponent<CameraComponent>(defaultCamera, {});
	addComponent<Transform>(defaultCamera, {});

	return defaultCamera;
}

void Scene::setMainScene(bool mainScene){
	this->mainScene = mainScene;
}

bool Scene::isMainScene(){
	return mainScene;
}

void Scene::setColor(Vector4 color){
	this->color = color;
}

void Scene::setColor(float red, float green, float blue, float alpha){
	setColor(Vector4(red, green, blue, alpha));
}

Vector4 Scene::getColor(){
	return color;
}

void Scene::setShadowsPCF(bool shadowsPCF){
	this->shadowsPCF = shadowsPCF;
}

bool Scene::isShadowsPCF(){
	return this->shadowsPCF;
}

void Scene::setRenderToTexture(bool renderToTexture){
	this->renderToTexture = renderToTexture;
}

bool Scene::isRenderToTexture(){
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

	// UI component
	if (entitySignature.test(getComponentType<UIComponent>())){
		auto ui = componentManager.getComponentArray<UIComponent>();
		ui->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());
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