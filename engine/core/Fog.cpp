#include "Fog.h"

using namespace Supernova;

Fog::Fog(){
    mode = S_FOG_LINEAR;
    linearStart = 200;
    linearEnd = 800;
    density = 1;
    visibility = 0.1;
    color = Vector3(0.8, 0.8, 0.8);
}

Fog::~Fog(){
    
}

void Fog::setMode(int mode){
    this->mode = mode;
}

void Fog::setLinearStart(float linearStart){
    this->linearStart = linearStart;
}

void Fog::setLinearEnd(float linearEnd){
    this->linearEnd = linearEnd;
}

void Fog::setDensity(float density){
    this->density = density;
}

void Fog::setVisibility(float visibility){
    this->visibility = visibility;
}

void Fog::setColor(Vector3 color){
    this->color = color;
}

int Fog::getMode(){
    return mode;
}

float Fog::getLinearStart(){
    return linearStart;
}

float Fog::getLinearEnd(){
    return linearEnd;
}

float Fog::getDensity(){
    return density;
}

float Fog::getVisibility(){
    return visibility;
}

Vector3 Fog::getColor(){
    return color;
}
