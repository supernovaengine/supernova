//
// (c) 2021 Eduardo Doria.
//

#include "Light.h"

#include "math/Angle.h"
#include <math.h>

using namespace Supernova;

Light::Light(Scene* scene): Object(scene){
    addComponent<LightComponent>({});

}

Light::~Light(){
    
}

void Light::setType(LightType type){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.type = type;
}

void Light::setDirection(Vector3 direction){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.direction = direction;
}

void Light::setColor(Vector3 color){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.color = color;
}

void Light::setRange(float range){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.range = range;
}

void Light::setIntensity(float intensity){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.intensity = intensity;
}

void Light::setConeAngle(float inner, float outer){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.innerConeCos = cos(Angle::defaultToRad(inner));
    lightcomp.outerConeCos = cos(Angle::defaultToRad(outer));
}