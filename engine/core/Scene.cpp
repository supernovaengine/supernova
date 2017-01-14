#include "Scene.h"

#include "Supernova.h"

#include "platform/Log.h"
#include "GUIObject.h"

Scene::Scene() {
    camera = NULL;
    childScene = NULL;
    userCamera = false;
    setAmbientLight(0.1);
    scene = this;
}

Scene::~Scene() {
    destroy();
}

void Scene::addLight (Light* light){    
    bool founded = false;
    
    std::vector<Light*>::iterator it;
    for (it = lights.begin(); it != lights.end(); ++it) {
        if (light == (*it))
            founded = true;
    }
    
    if (!founded){
        lights.push_back(light);
        sceneManager.setLights(lights);
    }
        
}

void Scene::removeLight (Light* light){
    std::vector<Light*>::iterator i = std::remove(lights.begin(), lights.end(), light);
    lights.erase(i,lights.end());
    sceneManager.setLights(lights);
}

void Scene::setAmbientLight(Vector3 ambientLight){
    this->ambientLight = ambientLight;
    sceneManager.setAmbientLight(ambientLight);
}

void Scene::setAmbientLight(const float ambientFactor){
    setAmbientLight(Vector3(ambientFactor, ambientFactor, ambientFactor));
}

Vector3 Scene::getAmbientLight(){
    return ambientLight;
}

void Scene::setCamera(Camera* camera){
    this->camera = camera;
    this->camera->setSceneObject(this);
    userCamera = true;
}

Camera* Scene::getCamera(){
    return camera;
}

void Scene::setChildScene(Scene* childScene){
    childScene->sceneManager.setChildScene(true);
    this->childScene = childScene;
}

Scene* Scene::getChildScene(){
    return childScene;
}

bool Scene::updateViewSize(){
    
    int viewX = 0;
    int viewY = 0;
    int viewWidth = Supernova::getScreenWidth();
    int viewHeight = Supernova::getScreenHeight();
    
    float screenAspect = (float)Supernova::getScreenWidth() / (float)Supernova::getScreenHeight();
    float canvasAspect = (float)Supernova::getPreferedCanvasWidth() / (float)Supernova::getPreferedCanvasHeight();
    
    //When canvas size is not changed
    if (Supernova::getScalingMode() == S_SCALING_LETTERBOX){
        if (screenAspect < canvasAspect){
            float aspect = (float)Supernova::getScreenWidth() / (float)Supernova::getPreferedCanvasWidth();
            int newHeight = (int)((float)Supernova::getPreferedCanvasHeight() * aspect);
            int dif = Supernova::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Supernova::getScreenHeight()-dif;
        }else{
            float aspect = (float)Supernova::getScreenHeight() / (float)Supernova::getPreferedCanvasHeight();
            int newWidth = (int)((float)Supernova::getPreferedCanvasWidth() * aspect);
            int dif = Supernova::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Supernova::getScreenWidth()-dif;
        }
    }
    
    if (Supernova::getScalingMode() == S_SCALING_CROP){
        if (screenAspect > canvasAspect){
            float aspect = (float)Supernova::getScreenWidth() / (float)Supernova::getPreferedCanvasWidth();
            int newHeight = (int)((float)Supernova::getPreferedCanvasHeight() * aspect);
            int dif = Supernova::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Supernova::getScreenHeight()-dif;
        }else{
            float aspect = (float)Supernova::getScreenHeight() / (float)Supernova::getPreferedCanvasHeight();
            int newWidth = (int)((float)Supernova::getPreferedCanvasWidth() * aspect);
            int dif = Supernova::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Supernova::getScreenWidth()-dif;
        }
    }

    
    bool status = sceneManager.viewSize(viewX, viewY, viewWidth, viewHeight);
    if (this->camera != NULL){
        camera->updateScreenSize();
    }

    if (childScene)
        childScene->updateViewSize();

    return status;
}

void Scene::doCamera(){
    if (this->camera == NULL){
        this->camera = new Camera(S_ORTHO);
        this->camera->setSceneObject(this);
    }
}

bool Scene::load(){

    sceneManager.load();
    Object::load();

    doCamera();

    Object::update();
    camera->update();

    if (childScene)
        childScene->load();

    return true;
}


bool Scene::draw(){
    
    sceneManager.draw();
    Object::draw();

    if (childScene)
        childScene->draw();

    return true;
}

void Scene::destroy(){
    if (!userCamera){
        delete camera;
    }
}
