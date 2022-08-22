//
// (c) 2021 Eduardo Doria.
//

#include "Light.h"

#include "util/Angle.h"
#include "util/Color.h"
#include <math.h>

using namespace Supernova;

Light::Light(Scene* scene): Object(scene){
    addComponent<LightComponent>({});

}

Light::~Light(){
    
}

void Light::setType(LightType type){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.type = type;
}

LightType Light::getType(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.type;
}

void Light::setDirection(Vector3 direction){
    LightComponent& lightcomp = getComponent<LightComponent>();
    Transform& transform = getComponent<Transform>();

    lightcomp.direction = direction;
    transform.needUpdate = true; //Does not affect children
}

void Light::setDirection(const float x, const float y, const float z){
    setDirection(Vector3(x,y,z));
}

Vector3 Light::getDirection(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.direction;
}

void Light::setColor(Vector3 color){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.color = Color::sRGBToLinear(color);
}

void Light::setColor(const float r, const float g, const float b){
    setColor(Vector3(r,g,b));
}

Vector3 Light::getColor(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.color;
}

void Light::setRange(float range){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.range = range;
}

float Light::getRange(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.range;
}

void Light::setIntensity(float intensity){
    LightComponent& lightcomp = getComponent<LightComponent>();
    Transform& transform = getComponent<Transform>();

    if (intensity > 0 && lightcomp.intensity == 0)
        transform.needUpdate = true; //Does not affect children

    lightcomp.intensity = intensity;
}

float  Light::getIntensity(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.intensity;
}

void Light::setConeAngle(float inner, float outer){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.innerConeCos = cos(Angle::defaultToRad(inner / 2));
    lightcomp.outerConeCos = cos(Angle::defaultToRad(outer / 2));
}

void Light::setInnerConeAngle(float inner){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.innerConeCos = cos(Angle::defaultToRad(inner / 2));
}

float Light::getInnerConeAngle(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.innerConeCos;
}

void Light::setOuterConeAngle(float outer){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.outerConeCos = cos(Angle::defaultToRad(outer / 2));
}

float Light::getOuterConeAngle(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.outerConeCos;
}

void Light::setShadows(bool shadows){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.shadows = shadows;
}

bool Light::isShadows(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.shadows;
}

void Light::setBias(float bias){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.shadowBias = bias;
}

float Light::getBias(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.shadowBias;
}

void Light::setShadowMapSize(unsigned int size){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.mapResolution = size;
}

unsigned int Light::getShadowMapSize(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.mapResolution;
}

void Light::setShadowCameraNearFar(float near, float far){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.shadowCameraNearFar = Vector2(near, far);
}

void Light::setCameraNear(float near){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.shadowCameraNearFar.x = near;
}

float Light::getCameraNear(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.shadowCameraNearFar.x;
}

void Light::setCameraFar(float far){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.shadowCameraNearFar.y = far;
}

float Light::getCameraFar(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.shadowCameraNearFar.y;
}

void Light::setNumCascades(unsigned int numCascades){
    LightComponent& lightcomp = getComponent<LightComponent>();

    lightcomp.numShadowCascades = numCascades;
}

float Light::getNumCascades(){
    LightComponent& lightcomp = getComponent<LightComponent>();

    return lightcomp.numShadowCascades;
}