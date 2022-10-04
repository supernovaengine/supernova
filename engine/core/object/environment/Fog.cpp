//
// (c) 2022 Eduardo Doria.
//

#include "Fog.h"
#include "util/Color.h"

using namespace Supernova;

Fog::Fog(){
    type = FogType::LINEAR;
    color = Color::sRGBToLinear(Vector3(0.8, 0.8, 0.8));
    density = 0.01;
    linearStart = 10;
    linearEnd = 60;
}

FogType Fog::getType() const{
    return type;
}

Vector3 Fog::getColor() const{
    return color;
}

float Fog::getDensity() const{
    return density;
}

float Fog::getLinearStart() const{
    return linearStart;
}

float Fog::getLinearEnd() const{
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

void Fog::setLinearStart(float start){
    this->linearStart = start;
}

void Fog::setLinearEnd(float end){
    this->linearEnd = end;
}

void Fog::setLinearStartEnd(float start, float end){
    this->linearStart = start;
    this->linearEnd = end;
}