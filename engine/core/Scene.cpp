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

    lightState = LightState::AUTO;
    globalIllumColor = Vector3(1.0, 1.0, 1.0);
    globalIllumIntensity = 0.1;

    enableUIEvents = true;
}

Scene::~Scene(){
    destroy();
}

void Scene::setCamera(Camera* camera){
    setCamera(camera->getEntity());
}

void Scene::setCamera(Entity camera){
    if (CameraComponent* cameracomp = findComponent<CameraComponent>(camera)){
        this->camera = camera;
        if (defaultCamera != NULL_ENTITY){
            destroyEntity(defaultCamera);
            defaultCamera = NULL_ENTITY;
        }
        cameracomp->needUpdate = true;
    }else{
        Log::error("Invalid camera entity: need CameraComponent");
    }
}

Entity Scene::getCamera() const{
    return camera;
}

Entity Scene::createDefaultCamera(){
    defaultCamera = createSystemEntity();
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
    if (this->shadowsPCF != shadowsPCF){
        this->shadowsPCF = shadowsPCF;
        getSystem<RenderSystem>()->needReloadMeshes();
    }
}

bool Scene::isShadowsPCF() const{
    return this->shadowsPCF;
}

void Scene::setLightState(LightState state){
    if (this->lightState != state){
        this->lightState = state;
        getSystem<RenderSystem>()->needReloadMeshes();
    }
}

LightState Scene::getLightState() const{
    return this->lightState;
}

void Scene::setGlobalIllumination(float intensity, Vector3 color){
    this->globalIllumIntensity = intensity;
    this->globalIllumColor = Color::sRGBToLinear(color);
}

void Scene::setGlobalIllumination(float intensity){
    this->globalIllumIntensity = intensity;
}

void Scene::setGlobalIllumination(Vector3 color){
    this->globalIllumColor = Color::sRGBToLinear(color);
}

float Scene::getGlobalIlluminationIntensity() const{
    return this->globalIllumIntensity;
}

Vector3 Scene::getGlobalIlluminationColor() const{
    return Color::linearTosRGB(this->globalIllumColor);
}

Vector3 Scene::getGlobalIlluminationColorLinear() const{
    return this->globalIllumColor;
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
