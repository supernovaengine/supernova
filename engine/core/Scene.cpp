#include "Scene.h"

#include "Engine.h"

#include "platform/Log.h"
#include "GUIObject.h"

Scene::Scene() {
    camera = NULL;
    childScene = false;
    useTransparency = false;
    useDepth = false;
    userCamera = false;
    setAmbientLight(0.1);
    scene = this;
    sky = NULL;
    fog = NULL;
}

Scene::~Scene() {
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

Vector3* Scene::getAmbientLight(){
    return &ambientLight;
}

std::vector<Light*>* Scene::getLights(){
    return &lights;
}

bool Scene::isChildScene(){
    return childScene;
}

bool Scene::isUseDepth(){
    return useDepth;
}

bool Scene::isUseTransparency(){
    return useTransparency;
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
    int viewWidth = Engine::getScreenWidth();
    int viewHeight = Engine::getScreenHeight();
    
    float screenAspect = (float)Engine::getScreenWidth() / (float)Engine::getScreenHeight();
    float canvasAspect = (float)Engine::getPreferedCanvasWidth() / (float)Engine::getPreferedCanvasHeight();
    
    //When canvas size is not changed
    if (Engine::getScalingMode() == S_SCALING_LETTERBOX){
        if (screenAspect < canvasAspect){
            float aspect = (float)Engine::getScreenWidth() / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = Engine::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Engine::getScreenHeight()-dif;
        }else{
            float aspect = (float)Engine::getScreenHeight() / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = Engine::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Engine::getScreenWidth()-dif;
        }
    }
    
    if (Engine::getScalingMode() == S_SCALING_CROP){
        if (screenAspect > canvasAspect){
            float aspect = (float)Engine::getScreenWidth() / (float)Engine::getPreferedCanvasWidth();
            int newHeight = (int)((float)Engine::getPreferedCanvasHeight() * aspect);
            int dif = Engine::getScreenHeight() - newHeight;
            viewY = (dif/2);
            viewHeight = Engine::getScreenHeight()-dif;
        }else{
            float aspect = (float)Engine::getScreenHeight() / (float)Engine::getPreferedCanvasHeight();
            int newWidth = (int)((float)Engine::getPreferedCanvasWidth() * aspect);
            int dif = Engine::getScreenWidth() - newWidth;
            viewX = (dif/2);
            viewWidth = Engine::getScreenWidth()-dif;
        }
    }

    
    bool status = sceneManager.getRender()->viewSize(Rect(viewX, viewY, viewWidth, viewHeight));
    if (this->camera != NULL){
        camera->updateAutomaticSizes();
    }
    
    std::vector<Scene*>::iterator it;
    for (it = subScenes.begin(); it != subScenes.end(); ++it) {
        (*it)->updateViewSize();
    }

    return status;
}

void Scene::doCamera(){
    if (this->camera == NULL){
        this->camera = new Camera(S_CAMERA_2D);
        this->camera->setSceneObject(this);
    }
}

void Scene::resetSceneProperties(){
    useTransparency = false;
    useDepth = false;
    if (camera->getProjection() == S_CAMERA_PERSPECTIVE){
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

    sceneManager.getRender()->setScene(this);

    doCamera();

    sceneManager.getRender()->load();
    resetSceneProperties();
    Object::load();

    Object::update();
    camera->update();

    return true;
}


bool Scene::draw(){
    
    transparentQueue.clear();
    
    sceneManager.getRender()->draw();
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
