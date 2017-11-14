#include "Light.h"

#include "math/Angle.h"
#include <stdlib.h>

using namespace Supernova;

Light::Light(){
    this->type = S_POINT_LIGHT;
    this->color = Vector3(1.0, 1.0, 1.0);
    this->target = Vector3(0.0, 0.0, 0.0);
    this->direction = Vector3(0.0, 0.0, 0.0);
    this->spotAngle = Angle::degToRad(20);
    this->power = 1;
    this->useShadow = true;
    this->shadowMap = NULL;
    this->shadowMapWidth = 1024;
    this->shadowMapHeight = 1024;
}

Light::Light(int type){
    this->type = type;
}

Light::~Light() {
    for (int i = 0; i < lightCameras.size(); i++){
        delete lightCameras[i];
    }
    lightCameras.clear();

    if (shadowMap)
        delete shadowMap;

    destroy();
}

int Light::getType(){
    return type;
}

Vector3 Light::getColor(){
    return color;
}

Vector3 Light::getTarget(){
    return target;
}

Vector3 Light::getDirection(){
    return direction;
}

float Light::getPower(){
    return power;
}

float Light::getSpotAngle(){
    return spotAngle;
}

bool Light::isUseShadow(){
    return useShadow;
}

Camera* Light::getLightCamera(){
    return lightCameras[0];
}

Camera* Light::getLightCamera(int index){
    return lightCameras[index];
}

Texture* Light::getShadowMap(){
    return shadowMap;
}

Matrix4 Light::getDepthVPMatrix(){
    return depthVPMatrix;
}

void Light::setPower(float power){
    this->power = power;
}

void Light::setColor(Vector3 color){
    this->color = color;
}

void Light::setUseShadow(bool useShadow){
    if (this->useShadow != useShadow){
        this->useShadow = useShadow;
        if (loaded)
            load();
    }
}

Vector3 Light::getWorldTarget(){
    return this->worldTarget;
}

void Light::updateLightCamera(){

    for (int i=0; i< lightCameras.size(); i++) {
        Vector3 cameraDirection = (lightCameras[i]->getPosition() - lightCameras[i]->getView()).normalize();
        if (cameraDirection == Vector3(0, 1, 0)) {
            lightCameras[i]->setUp(0, 0, 1);
        } else {
            lightCameras[i]->setUp(0, 1, 0);
        }
    }
}

void Light::updateMatrix(){
    Object::updateMatrix();

    worldTarget = modelMatrix * (target - position);

    if (useShadow && loaded){
        updateLightCamera();
    }
}

bool Light::loadShadow(){
    if (useShadow){
        shadowMap->load();
    }

    return true;
}
