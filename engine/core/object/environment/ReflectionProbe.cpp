// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ReflectionProbe.h"

using namespace doriax;

ReflectionProbe::ReflectionProbe(Scene* scene): Object(scene){
    addComponent<ReflectionProbeComponent>();
}

ReflectionProbe::ReflectionProbe(Scene* scene, Entity entity): Object(scene, entity){
}

ReflectionProbe::~ReflectionProbe(){
}

void ReflectionProbe::invalidateCapture(){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.needUpdate = true;
    probe.captureRevision++;
}

void ReflectionProbe::setMode(ReflectionProbeMode mode){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    if (probe.mode != mode){
        probe.mode = mode;
        invalidateCapture();
    }
}

ReflectionProbeMode ReflectionProbe::getMode() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.mode;
}

void ReflectionProbe::setUpdateMode(ReflectionProbeUpdateMode updateMode){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.updateMode = updateMode;
}

ReflectionProbeUpdateMode ReflectionProbe::getUpdateMode() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.updateMode;
}

void ReflectionProbe::setBoxOffset(Vector3 boxOffset){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.boxOffset = boxOffset;
}

void ReflectionProbe::setBoxOffset(const float x, const float y, const float z){
    setBoxOffset(Vector3(x, y, z));
}

Vector3 ReflectionProbe::getBoxOffset() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.boxOffset;
}

void ReflectionProbe::setBoxSize(Vector3 boxSize){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.boxSize = boxSize;
}

void ReflectionProbe::setBoxSize(const float x, const float y, const float z){
    setBoxSize(Vector3(x, y, z));
}

Vector3 ReflectionProbe::getBoxSize() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.boxSize;
}

void ReflectionProbe::setBlendDistance(float blendDistance){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.blendDistance = blendDistance;
}

float ReflectionProbe::getBlendDistance() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.blendDistance;
}

void ReflectionProbe::setIntensity(float intensity){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.intensity = intensity;
}

float ReflectionProbe::getIntensity() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.intensity;
}

void ReflectionProbe::setPriority(int priority){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.priority = priority;
}

int ReflectionProbe::getPriority() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.priority;
}

void ReflectionProbe::setResolution(unsigned int resolution){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    if (probe.resolution != resolution){
        probe.resolution = resolution;
        invalidateCapture();
    }
}

unsigned int ReflectionProbe::getResolution() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.resolution;
}

void ReflectionProbe::setClipPlanes(float nearClip, float farClip){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    if (probe.nearClip != nearClip || probe.farClip != farClip){
        probe.nearClip = nearClip;
        probe.farClip = farClip;
        invalidateCapture();
    }
}

void ReflectionProbe::setNearClip(float nearClip){
    setClipPlanes(nearClip, getFarClip());
}

float ReflectionProbe::getNearClip() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.nearClip;
}

void ReflectionProbe::setFarClip(float farClip){
    setClipPlanes(getNearClip(), farClip);
}

float ReflectionProbe::getFarClip() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.farClip;
}

void ReflectionProbe::setUpdateInterval(float updateInterval){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.updateInterval = updateInterval;
}

float ReflectionProbe::getUpdateInterval() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.updateInterval;
}

void ReflectionProbe::setIncludeSky(bool includeSky){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    if (probe.includeSky != includeSky){
        probe.includeSky = includeSky;
        invalidateCapture();
    }
}

bool ReflectionProbe::isIncludeSky() const{
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    return probe.includeSky;
}

void ReflectionProbe::setTextures(const std::string& texturePositiveX, const std::string& textureNegativeX,
                        const std::string& texturePositiveY, const std::string& textureNegativeY,
                        const std::string& texturePositiveZ, const std::string& textureNegativeZ){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.texture.setCubePaths(texturePositiveX, textureNegativeX, texturePositiveY, textureNegativeY, texturePositiveZ, textureNegativeZ);

    invalidateCapture();
}

void ReflectionProbe::setTexture(const std::string& texture){
    ReflectionProbeComponent& probe = getComponent<ReflectionProbeComponent>();

    probe.texture.setCubeMap(texture);

    invalidateCapture();
}

void ReflectionProbe::refresh(){
    invalidateCapture();
}
