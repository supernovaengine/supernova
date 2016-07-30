#include "Scene.h"

#include "Supernova.h"

#include "platform/Log.h"

Scene::Scene() {
    camera = NULL;
    userCamera = false;
    setAmbientLight(0.1);
}

Scene::~Scene() {
    destroy();
}

void Scene::addObject(Object* obj){
    baseObject.addObject(obj);
}

void Scene::addLight (Light* light){
    lights.push_back(light);
    sceneRender.setLights(lights);
    //TODO: colocar para nao repetir
    //Log::Error(LOG_TAG, "Object has a parent already");
}

void Scene::removeLight (Light* light){
    std::vector<Light*>::iterator i = std::remove(lights.begin(), lights.end(), light);
    lights.erase(i,lights.end());
    sceneRender.setLights(lights);
}

void Scene::setAmbientLight(Vector3 ambientLight){
    this->ambientLight = ambientLight;
    sceneRender.setAmbientLight(ambientLight);
}

void Scene::setAmbientLight(const float ambientFactor){
    setAmbientLight(Vector3(ambientFactor, ambientFactor, ambientFactor));
}

Vector3 Scene::getAmbientLight(){
    return ambientLight;
}

void Scene::setCamera(Camera* camera){
    this->camera = camera;
    this->camera->setSceneBaseObject(&baseObject);
    userCamera = true;
}

Camera* Scene::getCamera(){
    return camera;
}

bool Scene::screenSize(int width, int height){
    bool status = sceneRender.screenSize(width, height);
    if (this->camera != NULL){
        camera->updateScreenSize();
    }
    return status;
}

Object* Scene::getBaseObject(){
    return &baseObject;
}

void Scene::doCamera(){
    if (this->camera == NULL){
        this->camera = new Camera(S_ORTHO);
        this->camera->setSceneBaseObject(&baseObject);
    }
}

bool Scene::load(){

    sceneRender.load();
    baseObject.load();

    doCamera();

    baseObject.update();
    camera->update();

    return true;
}


bool Scene::draw(){

    sceneRender.draw();
    baseObject.draw();

    return true;
}

void Scene::destroy(){
    if (!userCamera){
        delete camera;
    }
}
