#include "DirectionalLight.h"
#include "math/Angle.h"

using namespace Supernova;

DirectionalLight::DirectionalLight(): Light(){
    type = S_DIRECTIONAL_LIGHT;
    power = 1;
}

DirectionalLight::~DirectionalLight(){

}

void DirectionalLight::updateLightCamera(){
    Light::updateLightCamera();
}

void DirectionalLight::setDirection(Vector3 direction){
    this->direction = direction;
}

void DirectionalLight::setDirection(float x, float y, float z){
    this->direction = Vector3(x, y, z);
}
