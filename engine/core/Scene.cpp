#include "Scene.h"

#include "Engine.h"

#include "platform/Log.h"
#include "GUIObject.h"
#include <stdlib.h>

using namespace Supernova;

Scene::Scene() {
    camera = NULL;
    childScene = false;
    useTransparency = false;
    useDepth = false;
    useLight = false;
    userCamera = false;
    setAmbientLight(0.1);
    scene = this;
    sky = NULL;
    fog = NULL;

    render = NULL;
    lightRender = NULL;
    fogRender = NULL;
    textureRender = NULL;
}

Scene::~Scene() {
    if (render)
        delete render;
    
    if (lightRender)
        delete lightRender;
    
    if (fogRender)
        delete fogRender;
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
    return render;
}

ObjectRender* Scene::getLightRender(){
    return lightRender;
}

ObjectRender* Scene::getFogRender(){
    return fogRender;
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

bool Scene::isUseLight(){
    return useLight;
}

bool Scene::isUseTransparency(){
    return useTransparency;
}

void Scene::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Object::updateVPMatrix(getCamera()->getViewMatrix(), getCamera()->getProjectionMatrix(), getCamera()->getViewProjectionMatrix(), new Vector3(getCamera()->getWorldPosition()));
}

void Scene::setCamera(Camera* camera){
    if (camera) {
        this->camera = camera;
        this->camera->setSceneObject(this);
        userCamera = true;
        if (loaded)
            this->camera->updateMatrix();
    }
}

Camera* Scene::getCamera(){
    return camera;
}

bool Scene::is3D(){
    if (camera){
        if (camera->getType() != S_CAMERA_2D)
            return true;
    }

    return false;
}

bool Scene::updateCameraSize(){

    if (!render)
        render = SceneRender::newInstance();

    Rect cameraRect;
    if (textureRender == NULL) {
        cameraRect = Rect(0, 0, Engine::getCanvasWidth(), Engine::getCanvasHeight());
    }else{
        cameraRect = Rect(0, 0, textureRender->getTextureFrameWidth(), textureRender->getTextureFrameHeight());
    }

    if (this->camera != NULL){
        camera->updateAutomaticSizes(cameraRect);
    }
    
    std::vector<Scene*>::iterator it;
    for (it = subScenes.begin(); it != subScenes.end(); ++it) {
        (*it)->updateCameraSize();
    }

    return true;
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
    if (camera->getType() == S_CAMERA_PERSPECTIVE){
        useDepth = true;
    }

    useLight = lightData.updateLights(getLights(), getAmbientLight());
}

void Scene::drawTransparentMeshes(){
    std::multimap<float, ConcreteObject*>::reverse_iterator it;
    for (it = transparentQueue.rbegin(); it != transparentQueue.rend(); ++it) {
        (*it).second->renderDraw();
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
        sky->renderDraw();
}

Texture* Scene::getTextureRender(){
    return textureRender;
}

void Scene::setTextureRender(Texture* textureRender){

    if (textureRender != NULL){

        char rand_id[10];
        static const char alphanum[] =
                "0123456789"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < 10; ++i) {
            rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        if (textureRender->getId() == "")
            textureRender->setId("scene|"+std::string(rand_id));

        if (textureRender->getType() == 0)
            textureRender->setType(S_TEXTURE_FRAME);

        if (textureRender->getTextureFrameWidth() == 0 || textureRender->getTextureFrameHeight() == 0){
            textureRender->setTextureFrameSize(512,512);
        }

        this->textureRender = textureRender;
    }else{
        this->textureRender = NULL;
    }
}

bool Scene::renderDraw(){
    if (textureRender == NULL) {
        render->viewSize(*Engine::getViewRect());
        if (!childScene)
            render->clear();
    }else{
        textureRender->getTextureRender()->initTextureFrame();

        render->viewSize(Rect(0, 0, textureRender->getTextureFrameWidth(), textureRender->getTextureFrameHeight()), false);
        render->clear();
    }

    transparentQueue.clear();

    render->setUseTransparency(isUseTransparency());
    render->setUseLight(isUseLight());
    render->setChildScene(isChildScene());
    render->setUseDepth(isUseDepth());

    bool drawreturn = render->draw();
    resetSceneProperties();

    Object::draw();
    drawSky();
    drawTransparentMeshes();
    drawChildScenes();

    if (textureRender != NULL) {
        textureRender->getTextureRender()->endTextureFrame();
    }

    return drawreturn;
}

bool Scene::draw() {

    Camera* originalCamera = this->camera;
    Texture* originalTextureRender = this->textureRender;

    this->setTextureRender(lights[0]->getShadowMap());
    this->setCamera(lights[0]->getCameraView());
    renderDraw();

    this->setCamera(originalCamera);
    this->setTextureRender(originalTextureRender);
    return renderDraw();

}

bool Scene::load(){

    if (!render)
        render = SceneRender::newInstance();

    render->setUseTransparency(isUseTransparency());
    render->setUseLight(isUseLight());
    render->setChildScene(isChildScene());
    render->setUseDepth(isUseDepth());

    doCamera();

    render->load();
    resetSceneProperties();

    if (useLight){
        if (!lightRender)
            lightRender = ObjectRender::newInstance();

        lightRender->addProperty(S_PROPERTY_AMBIENTLIGHT, S_PROPERTYDATA_FLOAT3, 1, ambientLight.ptr());

        lightRender->addProperty(S_PROPERTY_NUMPOINTLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numPointLight);
        lightRender->addProperty(S_PROPERTY_POINTLIGHT_POS, S_PROPERTYDATA_FLOAT3, lightData.numPointLight, &lightData.pointLightPos.front());
        lightRender->addProperty(S_PROPERTY_POINTLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numPointLight, &lightData.pointLightPower.front());
        lightRender->addProperty(S_PROPERTY_POINTLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numPointLight, &lightData.pointLightColor.front());

        lightRender->addProperty(S_PROPERTY_NUMSPOTLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numSpotLight);
        lightRender->addProperty(S_PROPERTY_SPOTLIGHT_POS, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightPos.front());
        lightRender->addProperty(S_PROPERTY_SPOTLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numSpotLight, &lightData.spotLightPower.front());
        lightRender->addProperty(S_PROPERTY_SPOTLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightColor.front());
        lightRender->addProperty(S_PROPERTY_SPOTLIGHT_TARGET, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightTarget.front());
        lightRender->addProperty(S_PROPERTY_SPOTLIGHT_CUTOFF, S_PROPERTYDATA_FLOAT1, lightData.numSpotLight, &lightData.spotLightCutOff.front());

        lightRender->addProperty(S_PROPERTY_NUMDIRLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numDirectionalLight);
        lightRender->addProperty(S_PROPERTY_DIRLIGHT_DIR, S_PROPERTYDATA_FLOAT3, lightData.numDirectionalLight, &lightData.directionalLightDir.front());
        lightRender->addProperty(S_PROPERTY_DIRLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numDirectionalLight, &lightData.directionalLightPower.front());
        lightRender->addProperty(S_PROPERTY_DIRLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numDirectionalLight, &lightData.directionalLightColor.front());
    }

    if (fog){
        if (!fogRender)
            fogRender = ObjectRender::newInstance();

        fogRender->addProperty(S_PROPERTY_FOG_MODE, S_PROPERTYDATA_INT1, 1, &(fog->mode));
        fogRender->addProperty(S_PROPERTY_FOG_COLOR, S_PROPERTYDATA_FLOAT3, 1, fog->color.ptr());
        fogRender->addProperty(S_PROPERTY_FOG_VISIBILITY, S_PROPERTYDATA_FLOAT1, 1, &(fog->visibility));
        fogRender->addProperty(S_PROPERTY_FOG_DENSITY, S_PROPERTYDATA_FLOAT1, 1, &(fog->density));
        fogRender->addProperty(S_PROPERTY_FOG_START, S_PROPERTYDATA_FLOAT1, 1, &(fog->linearStart));
        fogRender->addProperty(S_PROPERTY_FOG_END, S_PROPERTYDATA_FLOAT1, 1, &(fog->linearEnd));
    }


    bool loadreturn = Object::load();

    Object::updateMatrix();
    camera->updateMatrix();

    if (textureRender != NULL) {
        textureRender->load();
        updateCameraSize();
    }

    return loadreturn;
}

void Scene::destroy(){
    Object::destroy();

    if (!userCamera){
        delete camera;
    }
}
