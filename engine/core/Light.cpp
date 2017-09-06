#include "Light.h"

#include "math/Angle.h"
#include <stdlib.h>

using namespace Supernova;

Light::Light(){
    this->type = S_POINT_LIGHT;
    this->color = Vector3(1.0, 1.0, 1.0);
    this->target = Vector3(0.0, 0.0, 0.0);
    this->direction = Vector3(0.0, 0.0, 0.0);
    this->spotAngle = 20;
    this->power = 1;
    this->useShadow = true;
    this->shadowMapWidth = 512;
    this->shadowMapHeight = 512;
}

Light::Light(int type){
    this->type = type;
}

Light::~Light() {

    if (cameraView)
        delete cameraView;

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

Camera* Light::getCameraView(){
    return cameraView;
}

Texture* Light::getShadowMap(){
    return shadowMap;
}

Matrix4* Light::getDepthBiasMVP(){
    return &depthBiasMVP;
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

void Light::updateCameraView(){
    cameraView->setPosition(getWorldPosition());
    cameraView->setView(getWorldTarget());
    cameraView->setPerspective(spotAngle, (float)shadowMapWidth / (float)shadowMapHeight, 0.0001, 500);

    Vector3 cameraDirection = (cameraView->getPosition() - cameraView->getView()).normalize();
    if (cameraDirection == Vector3(0,1,0)){
        cameraView->setUp(0, 0, 1);
    }else{
        cameraView->setUp(0, 1, 0);
    }

    Matrix4 biasMatrix;
    biasMatrix.set(0, 0, 0.5);
    biasMatrix.set(0, 1, 0.0);
    biasMatrix.set(0, 2, 0.0);
    biasMatrix.set(0, 3, 0.5);

    biasMatrix.set(1, 0, 0.0);
    biasMatrix.set(1, 1, 0.5);
    biasMatrix.set(1, 2, 0.0);
    biasMatrix.set(1, 3, 0.5);

    biasMatrix.set(2, 0, 0.0);
    biasMatrix.set(2, 1, 0.0);
    biasMatrix.set(2, 2, 0.5);
    biasMatrix.set(2, 3, 0.5);

    biasMatrix.set(3, 0, 0.0);
    biasMatrix.set(3, 1, 0.0);
    biasMatrix.set(3, 2, 0.0);
    biasMatrix.set(3, 3, 1.0);

    Matrix4 modelMatrix2;
    modelMatrix2.set(0, 0, 1.0);
    modelMatrix2.set(0, 1, 1.0);
    modelMatrix2.set(0, 2, 1.0);
    modelMatrix2.set(0, 3, 1.0);

    modelMatrix2.set(1, 0, 1.0);
    modelMatrix2.set(1, 1, 1.0);
    modelMatrix2.set(1, 2, 1.0);
    modelMatrix2.set(1, 3, 1.0);

    modelMatrix2.set(2, 0, 1.0);
    modelMatrix2.set(2, 1, 1.0);
    modelMatrix2.set(2, 2, 1.0);
    modelMatrix2.set(2, 3, 1.0);

    modelMatrix2.set(3, 0, 1.0);
    modelMatrix2.set(3, 1, 1.0);
    modelMatrix2.set(3, 2, 1.0);
    modelMatrix2.set(3, 3, 1.0);

    depthBiasMVP = ((*cameraView->getProjectionMatrix()) * (*cameraView->getViewMatrix()) * modelMatrix);
}

void Light::updateMatrix(){
    Object::updateMatrix();

    worldTarget = modelMatrix * (target - position);

    if (useShadow && loaded){
        updateCameraView();
    }
}

bool Light::load(){
    if (useShadow){
        if (!cameraView)
            cameraView = new Camera();
        updateCameraView();

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

    return Object::load();
}
