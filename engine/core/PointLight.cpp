#include "PointLight.h"


using namespace Supernova;

PointLight::PointLight(): Light(){
    type = S_POINT_LIGHT;
    this->power = 50;
}

PointLight::~PointLight(){

}
