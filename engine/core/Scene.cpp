#include "Scene.h"

#include "Supernova.h"

#include "platform/Log.h"
#include "GUIObject.h"

Scene::Scene() {
    camera = NULL;
    isChildScene = false;
    useTransparency = false;
    useDepth = false;
    userCamera = false;
    setAmbientLight(0.1);
    scene = this;
    sky = NULL;
    fog = NULL;
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
    }
}

void Scene::removeLight (Light* light){
    std::vector<Light*>::iterator i = std::remove(lights.begin(), lights.end(), light);
    lights.erase(i,lights.end());
}

void Scene::addSubScene (Scene* scene){
    bool founded = false;
    
    std::vector<Scene*>::iterator it;
    for (it = subScenes.begin(); it != subScenes.end(); ++it) {
        if (scene == (*it))
            founded = true;
    }
    
    if (!founded){
        subScenes.push_back(scene);
    }
}

void Scene::removeSubScene (Scene* scene){
    std::vector<Scene*>::iterator i = std::remove(subScenes.begin(), subScenes.end(), scene);
    subScenes.erase(i,subScenes.end());
}

void Scene::addGUIObject (GUIObject* guiobject){
    bool founded = false;
    
    std::vector<GUIObject*>::iterator it;
    for (it = guiObjects.begin(); it != guiObjects.end(); ++it) {
        if (guiobject == (*it))
            founded = true;
    }
    
    if (!founded){
        guiObjects.push_back(guiobject);
    }
}

void Scene::removeGUIObject (GUIObject* guiobject){
    std::vector<GUIObject*>::iterator i = std::remove(guiObjects.begin(), guiObjects.end(), guiobject);
    guiObjects.erase(i,guiObjects.end());
}

SceneRender* Scene::getSceneRender(){
    return sceneManager.getRender();
}

void Scene::setSky(SkyBox* sky){
    this->sky = sky;
}

void Scene::setFog(Fog* fog){
    this->fog = fog;
}

void Scene::setAmbientLight(Vector3 ambientLight){
    this->ambientLight = ambientLight;
}

void Scene::setAmbientLight(const float ambientFactor){
    setAmbientLight(Vector3(ambientFactor, ambientFactor, ambientFactor));
}

Vector3 Scene::getAmbientLight(){
    return ambientLight;
}

void Scene::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Object::transform(getCamera()->getViewMatrix(), getCamera()->getProjectionMatrix(), getCamera()->getViewProjectionMatrix(), new Vector3(getCamera()->getWorldPosition()));
}

void Scene::setCamera(Camera* camera){
    this->camera = camera;
    this->camera->setSceneObject(this);
    userCamera = true;
}

Camera* Scene::getCamera(){
    return camera;
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
    
    std::vector<Scene*>::iterator it;
    for (it = subScenes.begin(); it != subScenes.end(); ++it) {
        (*it)->updateViewSize();
    }

    return status;
}

void Scene::doCamera(){
    if (this->camera == NULL){
        this->camera = new Camera(S_ORTHO);
        this->camera->setSceneObject(this);
    }
}

void Scene::resetSceneProperties(){
    useTransparency = false;
    useDepth = false;
    if (camera->getProjection() == S_PERSPECTIVE){
        useDepth = true;
    }
}

void Scene::drawTransparentMeshes(){
    std::multimap<float, ConcreteObject*>::reverse_iterator it;
    for (it = transparentQueue.rbegin(); it != transparentQueue.rend(); ++it) {
        (*it).second->render();
    }
}

void Scene::drawChildScenes(){
    std::vector<Scene*>::iterator it2;
    for (it2 = subScenes.begin(); it2 != subScenes.end(); ++it2) {
        (*it2)->draw();
    }
}

void Scene::drawSky(){
    if (sky != NULL)
        sky->render();
}

bool Scene::load(){

    sceneManager.getRender()->setChildScene(isChildScene);
    sceneManager.getRender()->setLights(&this->lights);
    sceneManager.getRender()->setAmbientLight(&this->ambientLight);
    sceneManager.getRender()->setFog(fog);

    doCamera();

    sceneManager.load();
    resetSceneProperties();
    Object::load();

    Object::update();
    camera->update();

    return true;
}


bool Scene::draw(){
    
    transparentQueue.clear();

    sceneManager.getRender()->setUseDepth(useDepth);
    sceneManager.getRender()->setUseTramsparency(useTransparency);
    
    sceneManager.draw();
    resetSceneProperties();
    Object::draw();
    drawSky();
    drawTransparentMeshes();

    drawChildScenes();
    return true;
}

void Scene::destroy(){
    if (!userCamera){
        delete camera;
    }
}
