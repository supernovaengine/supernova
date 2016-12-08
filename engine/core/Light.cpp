#include "Light.h"


Light::Light(){
    this->type = S_POINT_LIGHT;
    this->color = Vector3(1.0, 1.0, 1.0);
    this->target = Vector3(0.0, 0.0, 0.0);
    this->direction = Vector3(0.0, 0.0, 0.0);
    this->spotAngle = 20;
    this->power = 1;
    this->updated = true;
}

Light::Light(int type){
    this->type = type;
}

Light::~Light() {
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

void Light::setPower(float power){
    this->power = power;
}

void Light::setColor(Vector3 color){
    this->color = color;
}

Vector3 Light::getWorldTarget(){
    return this->worldTarget;
}

bool Light::isUpdated(){
    return updated;
}

void Light::setUpdated(bool updated){
    this->updated = updated;
}

void Light::update(){

    Object::update();

    worldTarget = modelMatrix * (target - position);
    
    updated = false;
}
