//
// (c) 2021 Eduardo Doria.
//

#include "Light.h"

#include "math/Angle.h"
#include "math/Color.h"
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
    Transform& transform = scene->getComponent<Transform>(entity);

    lightcomp.direction = direction;
    transform.needUpdate = true; //Does not affect children
}

void Light::setDirection(const float x, const float y, const float z){
    setDirection(Vector3(x,y,z));
}

void Light::setColor(Vector3 color){
    LightComponent& lightcomp = scene->getComponent<LightComponent>(entity);

    lightcomp.color = Color::sRGBToLinear(color);
}

void Light::setColor(const float r, const float g, const float b){
    setColor(Vector3(r,g,b));
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