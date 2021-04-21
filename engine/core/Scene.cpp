#include "Scene.h"

#include "object/Camera.h"
#include "Engine.h"

using namespace Supernova;

Scene::Scene(){
	registerComponent<MeshComponent>();
	registerComponent<SkyComponent>();
    registerComponent<Transform>();
	registerComponent<CameraComponent>();
	registerComponent<LightComponent>();

	registerSystem<RenderSystem>();

	camera = NULL_ENTITY;
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

void Scene::updateCameraSize(){
	CameraComponent* cameraComp = findComponent<CameraComponent>(camera);
	if (cameraComp){
		Rect rect;
    	//if (textureFrame == NULL) {
        	rect = Rect(0, 0, Engine::getCanvasWidth(), Engine::getCanvasHeight());
    	//}else{
    	//    rect = Rect(0, 0, textureFrame->getTextureFrameWidth(), textureFrame->getTextureFrameHeight());
    	//}

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

size_t Scene::findFamilyEndIndex(Entity entity){
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
		Log::Debug("Add child - BEFORE");
		for (int i = 0; i < transforms->size(); i++){
			auto transform = transforms->getComponentFromIndex(i);
			Log::Debug("Transform %i - Entity: %i - Parent: %i: %s", i, transforms->getEntity(i), transform.parent, transform.name.c_str());
		}
		//----------DEBUG

		//size_t parentIndex = transforms->getIndex(parent);
		size_t childIndex = transforms->getIndex(child);

		transformChild.parent = parent;

		//find children of parent and child family
		size_t newIndex = findFamilyEndIndex(parent) + 1;
		size_t lastChild = findFamilyEndIndex(child);

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
		Log::Debug("Add child - AFTER");
		for (int i = 0; i < transforms->size(); i++){
			auto transform = transforms->getComponentFromIndex(i);
			Log::Debug("Transform %i - Entity: %i - Parent: %i: %s", i, transforms->getEntity(i), transform.parent, transform.name.c_str());
		}
		Log::Debug("\n");
		//----------DEBUG
	}

	signature.set(componentManager.getComponentType<MeshComponent>(), true);

	if ( ((parentSignature & signature) == signature) || ((childSignature & signature) == signature) ){
		auto meshes = componentManager.getComponentArray<MeshComponent>();
		meshes->sortByComponent<Transform>(componentManager.getComponentArray<Transform>());

		//----------DEBUG
		Log::Debug("Add child (MESH)");
		for (int i = 0; i < meshes->size(); i++){
			auto transform = meshes->getComponentFromIndex(i);
			Log::Debug("Mesh %i - Entity: %i", i, meshes->getEntity(i));
		}
		Log::Debug("\n");
		//----------DEBUG
	}
}