//
// (c) 2022 Eduardo Doria.
//

#include "Fog.h"
#include "math/Color.h"

using namespace Supernova;

Fog::Fog(){
    type = FogType::LINEAR;
    color = Color::sRGBToLinear(Vector3(0.8, 0.8, 0.8));
    density = 0.001;
    linearStart = 100;
    linearEnd = 600;
}

FogType Fog::getType(){
    return type;
}

Vector3 Fog::getColor(){
    return color;
}

float Fog::getDensity(){
    return density;
}

float Fog::getLinearStart(){
    return linearStart;
}

float Fog::getLinearEnd(){
    return linearEnd;
}

void Fog::setType(FogType type){
    this->type = type;
}

void Fog::setColor(Vector3 color){
    this->color = Color::sRGBToLinear(color);
}

void Fog::setDensity(float density){
    this->density = density;
}

void Fog::setLinearStartEnd(float start, float end){
    this->linearStart = start;
    this->linearEnd = end;
}