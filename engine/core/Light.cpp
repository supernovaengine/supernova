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
    this->lightCamera = NULL;
    this->shadowMap = NULL;
    this->shadowMapWidth = 1024;
    this->shadowMapHeight = 1024;

    this->lightsCamera.clear();
}

Light::Light(int type){
    this->type = type;
}

Light::~Light() {

    if (lightCamera)
        delete lightCamera;

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
    return lightCamera;
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
    lightCamera->setPosition(getWorldPosition());
    lightCamera->setView(getWorldTarget());
    lightCamera->setPerspective(Angle::radToDefault(spotAngle), (float)shadowMapWidth / (float)shadowMapHeight, 1, 100 * power);

    Vector3 cameraDirection = (lightCamera->getPosition() - lightCamera->getView()).normalize();
    if (cameraDirection == Vector3(0,1,0)){
        lightCamera->setUp(0, 0, 1);
    }else{
        lightCamera->setUp(0, 1, 0);
    }

    depthVPMatrix = (*lightCamera->getViewProjectionMatrix());
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
        if (!lightCamera)
            lightCamera = new Camera();
        updateLightCamera();

        if (!shadowMap) {
            shadowMap = new Texture(shadowMapWidth, shadowMapHeight);

            char rand_id[10];
            static const char alphanum[] =
                    "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";
            for (int i = 0; i < 10; ++i) {
                rand_id[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
            }

            shadowMap->setId("shadowMap|" + std::string(rand_id));
            shadowMap->setType(S_TEXTURE_DEPTH_FRAME);
        }
        shadowMap->load();
    }

    return true;
}
