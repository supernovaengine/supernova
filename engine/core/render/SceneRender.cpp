#include "SceneRender.h"
#include "math/Angle.h"
#include "Scene.h"

SceneRender::SceneRender(){
    lighting = false;
    childScene = false;
    useDepth = false;
    useTransparency = false;

    lights = NULL;
    ambientLight = NULL;
    fog = NULL;
}

SceneRender::~SceneRender(){
}

void SceneRender::setScene(Scene* scene){
    this->scene = scene;
}

void SceneRender::fillSceneProperties(){
    this->lights = scene->getLights();
    this->ambientLight = scene->getAmbientLight();
    this->childScene = scene->isChildScene();
    this->useDepth = scene->isUseDepth();
    this->useTransparency = scene->isUseTransparency();
}

LightRender* SceneRender::getLightRender(){
    return &lightRender;
}

Fog* SceneRender::getFog(){
    return fog;
}

bool SceneRender::load(){
    fillSceneProperties();
    lighting = lightRender.updateLights(lights, ambientLight);

    return true;
}
bool SceneRender::draw(){
    fillSceneProperties();
    lighting = lightRender.updateLights(lights, ambientLight);

    return true;
}
