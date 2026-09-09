// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Light2D.h"

#include "util/Color.h"
#include "subsystem/RenderSystem.h"

using namespace doriax;

Light2D::Light2D(Scene* scene): Object(scene){
    addComponent<Light2DComponent>();
}

Light2D::Light2D(Scene* scene, Entity entity): Object(scene, entity){
}

Light2D::~Light2D(){
}

void Light2D::setColor(Vector3 color){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.color = Color::sRGBToLinear(color);
}

void Light2D::setColor(const float r, const float g, const float b){
    setColor(Vector3(r,g,b));
}

Vector3 Light2D::getColor() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.color;
}

void Light2D::setIntensity(float intensity){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.intensity = intensity;
}

float Light2D::getIntensity() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.intensity;
}

void Light2D::setRange(float range){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.range = range;
}

float Light2D::getRange() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.range;
}

void Light2D::setFalloff(float falloff){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.falloff = falloff;
}

float Light2D::getFalloff() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.falloff;
}

void Light2D::setHeight(float height){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.height = height;
}

float Light2D::getHeight() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.height;
}

void Light2D::setShadows(bool shadows){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    if (lightcomp.shadows != shadows){
        lightcomp.shadows = shadows;

        // USE_SHADOWS_2D is a shader variant, so meshes must rebuild
        scene->getSystem<RenderSystem>()->needReloadMeshes();
    }
}

bool Light2D::isShadows() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.shadows;
}

void Light2D::setShadowBias(float shadowBias){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.shadowBias = shadowBias;
}

float Light2D::getShadowBias() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.shadowBias;
}

void Light2D::setShadowSoftness(float shadowSoftness){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.shadowSoftness = shadowSoftness;
}

float Light2D::getShadowSoftness() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.shadowSoftness;
}

void Light2D::setMapResolution(unsigned int mapResolution){
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    lightcomp.mapResolution = mapResolution;
}

unsigned int Light2D::getMapResolution() const{
    Light2DComponent& lightcomp = getComponent<Light2DComponent>();

    return lightcomp.mapResolution;
}
