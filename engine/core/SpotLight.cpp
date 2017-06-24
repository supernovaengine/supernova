#include "SpotLight.h"

using namespace Supernova;

SpotLight::SpotLight(): Light(){
    type = S_SPOT_LIGHT;
    this->power = 50;
}

SpotLight::~SpotLight(){

}

void SpotLight::setTarget(Vector3 target){
    if (this->target != target){
        this->target = target;
        updateMatrix();
    }
}

void SpotLight::setTarget(float x, float y, float z){
    setTarget(Vector3(x, y, z));
}

void SpotLight::setSpotAngle(float angle){
    this->spotAngle = angle;
}
