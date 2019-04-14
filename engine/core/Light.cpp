#include "Light.h"

#include "math/Angle.h"
#include <stdlib.h>
#include "Scene.h"

using namespace Supernova;

Light::Light(){
    this->type = S_POINT_LIGHT;
    this->color = Vector3(1.0, 1.0, 1.0);
    this->target = Vector3(0.0, 0.0, 0.0);
    this->direction = Vector3(0.0, 0.0, 0.0);
    this->spotAngle = Angle::degToRad(20);
    this->spotOuterAngle = Angle::degToRad(20);
    this->power = 1;
    this->useShadow = false;
    this->shadowMapWidth = 1024;
    this->shadowMapHeight = 1024;
    this->shadowBias = 0.001;

    this->shadowMap.clear();
    this->lightCameras.clear();
    this->depthVPMatrix.clear();
}

Light::Light(int type){
    this->type = type;
}

Light::~Light() {
    for (int i = 0; i < lightCameras.size(); i++){
        delete lightCameras[i];
    }
    lightCameras.clear();

    if (shadowMap.size() > 0)
        for (int i = 0; i < shadowMap.size(); i++)
            delete shadowMap[i];

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

float Light::getSpotOuterAngle(){
    return spotOuterAngle;
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
    return shadowMap[0];
}

Texture* Light::getShadowMap(int index){
    return shadowMap[index];
}

Matrix4 Light::getDepthVPMatrix(){
    return depthVPMatrix[0];
}

Matrix4 Light::getDepthVPMatrix(int index){
    return depthVPMatrix[index];
}

void Light::setPower(float power){
    this->power = power;
}

void Light::setColor(Vector3 color){
    this->color = color;
}

void Light::setShadow(bool useShadow){
    if (this->useShadow != useShadow){
        this->useShadow = useShadow;
        if (loaded)
            load();
    }
}

void Light::setShadowBias(float shadowBias){
    if (this->shadowBias != shadowBias){
        this->shadowBias = shadowBias;
        if (loaded)
            load();
    }
}

Vector3 Light::getWorldTarget(){
    return this->worldTarget;
}

float Light::getShadowBias(){
    return shadowBias;
}

void Light::updateLightCamera(){

}

void Light::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    if (scene && !scene->isDrawingShadow())
        Object::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);
}

void Light::updateModelMatrix(){
    Object::updateModelMatrix();

    worldTarget = modelMatrix * (target - position);

    if (useShadow && loaded){
        updateLightCamera();
    }
}

bool Light::loadShadow(){
    if (useShadow){
        if (shadowMap.size() > 0)
            for (int i = 0; i < shadowMap.size(); i++)
                shadowMap[i]->load();
    }

    return true;
}
