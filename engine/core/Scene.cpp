#include "Scene.h"

#include "Engine.h"
#include "DirectionalLight.h"
#include "physics/PhysicsWorld2D.h"

#include "Log.h"
#include "ui/UIObject.h"
#include <stdlib.h>

using namespace Supernova;

Scene::Scene() {
    camera = NULL;
    loadedShadow = false;
    drawingShadow = false;
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
    textureFrame = NULL;

    physicsWorld = NULL;
    ownedPhysicsWorld = true;

    drawShadowLightPos = Vector3();
    drawShadowCameraNearFar = Vector2();
    drawIsPointShadow = false;

    userDefinedTransparency = S_OPTION_AUTOMATIC;
    userDefinedDepth = S_OPTION_AUTOMATIC;
}

Scene::~Scene() {
    if (render)
        delete render;

    if (ownedPhysicsWorld)
        delete physicsWorld;
    else
        physicsWorld->attachedScene = NULL;
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
    lights.erase(i, lights.end());
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
    subScenes.erase(i, subScenes.end());
}

void Scene::addGUIObject (UIObject* guiobject){
    bool founded = false;
    
    std::vector<UIObject*>::iterator it;
    for (it = guiObjects.begin(); it != guiObjects.end(); ++it) {
        if (guiobject == (*it))
            founded = true;
    }
    
    if (!founded){
        guiObjects.push_back(guiobject);
    }
}

void Scene::removeGUIObject (UIObject* guiobject){
    std::vector<UIObject*>::iterator i = std::remove(guiObjects.begin(), guiObjects.end(), guiobject);
    guiObjects.erase(i, guiObjects.end());
}

void Scene::setOwnedPhysicsWorld(bool ownedPhysicsWorld){
    this->ownedPhysicsWorld = ownedPhysicsWorld;
}

PhysicsWorld2D* Scene::createPhysicsWorld2D(){
    physicsWorld = new PhysicsWorld2D();
    return (PhysicsWorld2D*)getPhysicsWorld();
}

//void Scene::createPhysicsWorld3D(){
//    physicsWorld = new PhysicsWorld3D();
//}

void Scene::setPhysicsWorld (PhysicsWorld* physicsWorld){
    if (!physicsWorld && this->physicsWorld) {

        this->physicsWorld->attachedScene = NULL;
        this->physicsWorld = NULL;

    }else if (this->physicsWorld != physicsWorld) {

        if (ownedPhysicsWorld)
            delete this->physicsWorld;

        this->physicsWorld = physicsWorld;

    }
}
PhysicsWorld* Scene::getPhysicsWorld(){
    if (!physicsWorld)
        Log::Error("Physics is not created on scene");

    return physicsWorld;
}

SceneRender* Scene::getSceneRender(){
    return render;
}

LightData* Scene::getLightData(){
    return &lightData;
}

Vector3 Scene::getDrawShadowLightPos(){
    return drawShadowLightPos;
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

bool Scene::isLoadedShadow(){
    return loadedShadow;
}


bool Scene::isDrawingShadow(){
    return drawingShadow;
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

void Scene::setTransparency(bool transparency){
    if (transparency)
        userDefinedTransparency = S_OPTION_YES;
    else
        userDefinedTransparency = S_OPTION_NO;
}

void Scene::setDepth(bool depth){
    if (depth)
        userDefinedDepth = S_OPTION_YES;
    else
        userDefinedDepth = S_OPTION_NO;
}

int Scene::getUserDefinedTransparency(){
    return userDefinedTransparency;
}

int Scene::getUserDefinedDepth(){
    return userDefinedDepth;
}

void Scene::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Object::updateVPMatrix(getCamera()->getViewMatrix(), getCamera()->getProjectionMatrix(), getCamera()->getViewProjectionMatrix(), new Vector3(getCamera()->getWorldPosition()));
}

void Scene::setCamera(Camera* camera){
    if (camera && (camera != this->camera)) {
        if (this->camera)
            this->camera->setLinkedScene(NULL);
        this->camera = camera;
        this->camera->setLinkedScene(this);
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
    if (textureFrame == NULL) {
        cameraRect = Rect(0, 0, Engine::getCanvasWidth(), Engine::getCanvasHeight());
    }else{
        cameraRect = Rect(0, 0, textureFrame->getTextureFrameWidth(), textureFrame->getTextureFrameHeight());
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
        this->camera->setLinkedScene(this);
    }
}

void Scene::resetSceneProperties(){
    useTransparency = false;
    useDepth = false;

    if (camera->getType() == S_CAMERA_PERSPECTIVE && userDefinedDepth == S_OPTION_AUTOMATIC) {
        useDepth = true;
    }else if (userDefinedDepth == S_OPTION_YES){
        useDepth = true;
    }
    if (userDefinedTransparency == S_OPTION_YES){
        useTransparency = true;
    }

    useLight = lightData.updateLights(getLights(), getAmbientLight());
}

void Scene::drawTransparentMeshes(){
    std::multimap<float, GraphicObject*>::reverse_iterator it;
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

Texture* Scene::getTextureFrame(){
    return textureFrame;
}

void Scene::setTextureFrame(Texture* textureFrame){

    if (textureFrame != NULL){

        char rand_id[10];
        static const char alphanum[] =
                "0123456789"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < 10; ++i) {
            rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        if (textureFrame->getId() == "")
            textureFrame->setId("scene|"+std::string(rand_id));

        if (textureFrame->getType() == S_TEXTURE_2D)
            textureFrame->setType(S_TEXTURE_FRAME);

        if (textureFrame->getTextureFrameWidth() == 0 || textureFrame->getTextureFrameHeight() == 0){
            textureFrame->setTextureFrameSize(512,512);
        }

        this->textureFrame = textureFrame;
    }else{
        this->textureFrame = NULL;
    }
}

bool Scene::renderDraw(bool shadowMap, bool cubeMap, int cubeFace){
    if (textureFrame == NULL) {
        render->viewSize(*Engine::getViewRect());
        if (!childScene)
            render->clear();
    }else{
        if (cubeMap) {
            textureFrame->getTextureRender()->initTextureFrame(cubeFace);
        }else{
            textureFrame->getTextureRender()->initTextureFrame();
        }

        render->viewSize(Rect(0, 0, textureFrame->getTextureFrameWidth(), textureFrame->getTextureFrameHeight()));
        if (shadowMap)
            render->clear(1.0);
        else
            render->clear();
    }

    transparentQueue.clear();
    
    if (!drawingShadow) {
        render->setUseTransparency(isUseTransparency());
        render->setUseLight(isUseLight());
        render->setChildScene(isChildScene());
        render->setUseDepth(isUseDepth());

        resetSceneProperties();
    }else{
        render->setUseTransparency(false);
        render->setUseLight(false);
        render->setChildScene(false);
        render->setUseDepth(true);
    }
    render->setDrawingShadow(drawingShadow);

    bool drawreturn = render->draw();

    Object::draw();
    if (!drawingShadow) {
        drawSky();
        drawTransparentMeshes();
        drawChildScenes();
    }

    if (textureFrame != NULL) {
        textureFrame->getTextureRender()->endTextureFrame();
    }

    return drawreturn;
}

void Scene::updatePhysics(float time){
    if (physicsWorld) {
        physicsWorld->step(time);
        physicsWorld->updateBodyObjects();
    }
}

bool Scene::draw() {
    //TODO: alert if not loaded

    Camera* originalCamera = this->camera;
    Texture* originalTextureRender = this->textureFrame;

    for (int i=0; i<lights.size(); i++) {
        if (lights[i]->isUseShadow()) {
            drawingShadow = true;

            this->drawShadowLightPos = lights[i]->getPosition();

            if (lights[i]->getType() == S_POINT_LIGHT) {

                this->drawIsPointShadow = true;

                this->setTextureFrame(lights[i]->getShadowMap());
                for (int cam = 0; cam < 6; cam++){
                    this->setCamera(lights[i]->getLightCamera(cam));
                    this->drawShadowCameraNearFar = lights[i]->getLightCamera(cam)->getNearFarPlane();

                    renderDraw(true, true, TEXTURE_CUBE_FACE_POSITIVE_X + cam);
                }

            }else if (lights[i]->getType() == S_SPOT_LIGHT) {

                this->drawIsPointShadow = false;

                this->setTextureFrame(lights[i]->getShadowMap());
                this->setCamera(lights[i]->getLightCamera());
                this->drawShadowCameraNearFar = lights[i]->getLightCamera()->getNearFarPlane();

                renderDraw(true);

            }else if (lights[i]->getType() == S_DIRECTIONAL_LIGHT) {

                this->drawIsPointShadow = false;

                for (int ca = 0; ca < ((DirectionalLight*)lights[i])->getNumShadowCasdades(); ca++) {
                    this->setTextureFrame(lights[i]->getShadowMap(ca));
                    this->setCamera(lights[i]->getLightCamera(ca));
                    this->drawShadowCameraNearFar = lights[i]->getLightCamera(ca)->getNearFarPlane();

                    renderDraw(true);
                }

            }

        }
    }

    if (drawingShadow) {
        drawingShadow = false;
        this->setCamera(originalCamera);
        this->setTextureFrame(originalTextureRender);
    }

    return renderDraw();

}

bool Scene::addLightProperties(ObjectRender* render){
    if (useLight){

        render->addProperty(S_PROPERTY_AMBIENTLIGHT, S_PROPERTYDATA_FLOAT3, 1, &ambientLight);

        render->addProperty(S_PROPERTY_NUMPOINTLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numPointLight);
        render->addProperty(S_PROPERTY_POINTLIGHT_POS, S_PROPERTYDATA_FLOAT3, lightData.numPointLight, &lightData.pointLightPos.front());
        render->addProperty(S_PROPERTY_POINTLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numPointLight, &lightData.pointLightPower.front());
        render->addProperty(S_PROPERTY_POINTLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numPointLight, &lightData.pointLightColor.front());
        render->addProperty(S_PROPERTY_POINTLIGHT_SHADOWIDX, S_PROPERTYDATA_INT1, lightData.numPointLight, &lightData.pointLightShadowIdx.front());

        render->addProperty(S_PROPERTY_NUMSPOTLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numSpotLight);
        render->addProperty(S_PROPERTY_SPOTLIGHT_POS, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightPos.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numSpotLight, &lightData.spotLightPower.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightColor.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_TARGET, S_PROPERTYDATA_FLOAT3, lightData.numSpotLight, &lightData.spotLightTarget.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_CUTOFF, S_PROPERTYDATA_FLOAT1, lightData.numSpotLight, &lightData.spotLightCutOff.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_OUTERCUTOFF, S_PROPERTYDATA_FLOAT1, lightData.numSpotLight, &lightData.spotLightOuterCutOff.front());
        render->addProperty(S_PROPERTY_SPOTLIGHT_SHADOWIDX, S_PROPERTYDATA_INT1, lightData.numSpotLight, &lightData.spotLightShadowIdx.front());

        render->addProperty(S_PROPERTY_NUMDIRLIGHT, S_PROPERTYDATA_INT1, 1, &lightData.numDirectionalLight);
        render->addProperty(S_PROPERTY_DIRLIGHT_DIR, S_PROPERTYDATA_FLOAT3, lightData.numDirectionalLight, &lightData.directionalLightDir.front());
        render->addProperty(S_PROPERTY_DIRLIGHT_POWER, S_PROPERTYDATA_FLOAT1, lightData.numDirectionalLight, &lightData.directionalLightPower.front());
        render->addProperty(S_PROPERTY_DIRLIGHT_COLOR, S_PROPERTYDATA_FLOAT3, lightData.numDirectionalLight, &lightData.directionalLightColor.front());
        render->addProperty(S_PROPERTY_DIRLIGHT_SHADOWIDX, S_PROPERTYDATA_INT1, lightData.numDirectionalLight, &lightData.directionalLightShadowIdx.front());

        return true;
    }
    return false;
}

bool Scene::addFogProperties(ObjectRender* render){
    if (fog){
        render->addProperty(S_PROPERTY_FOG_MODE, S_PROPERTYDATA_INT1, 1, &(fog->mode));
        render->addProperty(S_PROPERTY_FOG_COLOR, S_PROPERTYDATA_FLOAT3, 1, &(fog->color));
        render->addProperty(S_PROPERTY_FOG_VISIBILITY, S_PROPERTYDATA_FLOAT1, 1, &(fog->visibility));
        render->addProperty(S_PROPERTY_FOG_DENSITY, S_PROPERTYDATA_FLOAT1, 1, &(fog->density));
        render->addProperty(S_PROPERTY_FOG_START, S_PROPERTYDATA_FLOAT1, 1, &(fog->linearStart));
        render->addProperty(S_PROPERTY_FOG_END, S_PROPERTYDATA_FLOAT1, 1, &(fog->linearEnd));

        return true;
    }
    return false;
}

bool Scene::load(){

    if (!render)
        render = SceneRender::newInstance();

    render->setUseTransparency(isUseTransparency());
    render->setUseLight(isUseLight());
    render->setChildScene(isChildScene());
    render->setUseDepth(isUseDepth());

    loadedShadow = false;
    for (int i=0; i<lights.size(); i++) {
        if (lights[i]->isUseShadow()) {
            lights[i]->loadShadow();
            loadedShadow = true;
        }
    }

    doCamera();

    render->load();
    resetSceneProperties();

    bool loadreturn = Object::load();

    camera->updateMatrix();
    Object::updateMatrix();

    if (textureFrame != NULL) {
        textureFrame->load();
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
