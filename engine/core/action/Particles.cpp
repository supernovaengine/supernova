// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Particles.h"

#include "subsystem/ActionSystem.h"

using namespace doriax;

Particles::Particles(Scene* scene): Action(scene){
    addComponent<ParticlesComponent>();
}

Particles::Particles(Scene* scene, Entity entity): Action(scene, entity){

}

Particles::~Particles(){

}

void Particles::reset(){
    scene->getSystem<ActionSystem>()->particleActionReset(getEntity());
}

void Particles::setMaxParticles(unsigned int maxParticles){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    if (particomp.maxParticles != maxParticles){
        particomp.maxParticles = maxParticles;

        //particomp.needReload = true;
    }
}

unsigned int Particles::getMaxParticles() const{
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    return particomp.maxParticles;
}

void Particles::setRate(int rate){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();
    
    partAnim.rate = rate;
}

int Particles::getRate() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();
    
    return partAnim.rate;
}

void Particles::setEmitter(bool emitter){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.emitter = emitter;
}

bool Particles::isEmitter() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    return partAnim.emitter;
}

void Particles::setLoop(bool loop){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.loop = loop;
}

bool Particles::isLoop() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    return partAnim.loop;
}

void Particles::setLocalSpace(bool localSpace){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.localSpace = localSpace;
}

bool Particles::isLocalSpace() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    return partAnim.localSpace;
}

void Particles::setMaxPerUpdate(int maxPerUpdate){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();
    
    partAnim.maxPerUpdate = maxPerUpdate;
}

int Particles::getMaxPerUpdate() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();
    
    return partAnim.maxPerUpdate;
}

void Particles::setLifeInitializer(float life){
    setLifeInitializer(life, life);
}

void Particles::setLifeInitializer(float minLife, float maxLife){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.lifeInitializer.minLife = minLife;
    partAnim.lifeInitializer.maxLife = maxLife;
}

void Particles::setPositionInitializer(Vector3 position){
    setPositionInitializer(position, position);
}

void Particles::setPositionInitializer(Vector3 minPosition, Vector3 maxPosition){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = ParticleEmitterShape::Box;
    partAnim.positionInitializer.minPosition = minPosition;
    partAnim.positionInitializer.maxPosition = maxPosition;
}

void Particles::setPositionInitializerShape(ParticleEmitterShape shape){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = shape;
}

ParticleEmitterShape Particles::getPositionInitializerShape() const{
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    return partAnim.positionInitializer.shape;
}

void Particles::setSpherePositionInitializer(float radius){
    setSpherePositionInitializer(radius, 0.0f);
}

void Particles::setSpherePositionInitializer(float radius, float innerRadius){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = ParticleEmitterShape::Sphere;
    partAnim.positionInitializer.radius = radius;
    partAnim.positionInitializer.innerRadius = innerRadius;
}

void Particles::setHemispherePositionInitializer(float radius){
    setHemispherePositionInitializer(radius, 0.0f);
}

void Particles::setHemispherePositionInitializer(float radius, float innerRadius){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = ParticleEmitterShape::Hemisphere;
    partAnim.positionInitializer.radius = radius;
    partAnim.positionInitializer.innerRadius = innerRadius;
}

void Particles::setCirclePositionInitializer(float radius){
    setCirclePositionInitializer(radius, 0.0f);
}

void Particles::setCirclePositionInitializer(float radius, float innerRadius){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = ParticleEmitterShape::Circle;
    partAnim.positionInitializer.radius = radius;
    partAnim.positionInitializer.innerRadius = innerRadius;
}

void Particles::setConePositionInitializer(float angle, float height){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionInitializer.shape = ParticleEmitterShape::Cone;
    partAnim.positionInitializer.coneAngle = angle;
    partAnim.positionInitializer.coneHeight = height;
}

void Particles::addBurst(float time, int count){
    addBurst(time, count, count);
}

void Particles::addBurst(float time, int minCount, int maxCount){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    ParticleBurst burst;
    burst.time = time;
    burst.minCount = minCount;
    burst.maxCount = maxCount;
    partAnim.bursts.push_back(burst);
    partAnim.currentBurst = 0;
}

void Particles::setBursts(std::vector<ParticleBurst> bursts){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.bursts = bursts;
    partAnim.currentBurst = 0;
}

void Particles::clearBursts(){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.bursts.clear();
    partAnim.currentBurst = 0;
}

void Particles::setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition){
    setPositionModifier(fromTime, toTime, fromPosition, toPosition, EaseType::LINEAR);
}

void Particles::setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition, EaseType functionType){
    setPositionModifier(fromTime, toTime, fromPosition, toPosition, Ease(functionType));
}

void Particles::setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.positionModifier.fromTime = fromTime;
    partAnim.positionModifier.toTime = toTime;
    partAnim.positionModifier.fromPosition = fromPosition;
    partAnim.positionModifier.toPosition = toPosition;
    partAnim.positionModifier.function = function;
}

void Particles::setVelocityInitializer(Vector3 velocity){
    setVelocityInitializer(velocity, velocity);
}

void Particles::setVelocityInitializer(Vector3 minVelocity, Vector3 maxVelocity){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.velocityInitializer.minVelocity = minVelocity;
    partAnim.velocityInitializer.maxVelocity = maxVelocity;
}

void Particles::setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity){
    setVelocityModifier(fromTime, toTime, fromVelocity, toVelocity, EaseType::LINEAR);
}

void Particles::setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity, EaseType functionType){
    setVelocityModifier(fromTime, toTime, fromVelocity, toVelocity, Ease(functionType));
}

void Particles::setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.velocityModifier.fromTime = fromTime;
    partAnim.velocityModifier.toTime = toTime;
    partAnim.velocityModifier.fromVelocity = fromVelocity;
    partAnim.velocityModifier.toVelocity = toVelocity;
    partAnim.velocityModifier.function = function;
}

void Particles::setAccelerationInitializer(Vector3 acceleration){
    setAccelerationInitializer(acceleration, acceleration);
}

void Particles::setAccelerationInitializer(Vector3 minAcceleration, Vector3 maxAcceleration){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.accelerationInitializer.minAcceleration = minAcceleration;
    partAnim.accelerationInitializer.maxAcceleration = maxAcceleration;
}

void Particles::setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration){
    setAccelerationModifier(fromTime, toTime, fromAcceleration, toAcceleration, EaseType::LINEAR);
}

void Particles::setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration, EaseType functionType){
    setAccelerationModifier(fromTime, toTime, fromAcceleration, toAcceleration, Ease(functionType));
}

void Particles::setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.accelerationModifier.fromTime = fromTime;
    partAnim.accelerationModifier.toTime = toTime;
    partAnim.accelerationModifier.fromAcceleration = fromAcceleration;
    partAnim.accelerationModifier.toAcceleration = toAcceleration;
    partAnim.accelerationModifier.function = function;
}

void Particles::setColorInitializer(Vector3 color){
    setColorInitializer(color, color);
}

void Particles::setColorInitializer(Vector3 minColor, Vector3 maxColor){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.colorInitializer.minColor = minColor;
    partAnim.colorInitializer.maxColor = maxColor;
}

void Particles::setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor){
    setColorModifier(fromTime, toTime, fromColor, toColor, EaseType::LINEAR);
}

void Particles::setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor, EaseType functionType){
    setColorModifier(fromTime, toTime, fromColor, toColor, Ease(functionType));
}

void Particles::setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.colorModifier.fromTime = fromTime;
    partAnim.colorModifier.toTime = toTime;
    partAnim.colorModifier.fromColor = fromColor;
    partAnim.colorModifier.toColor = toColor;
    partAnim.colorModifier.function = function;
}

void Particles::addColorGradientStop(float time, Vector3 color){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    ParticleColorGradientStop stop;
    stop.time = time;
    stop.color = color;
    partAnim.colorGradient.stops.push_back(stop);
    partAnim.colorGradient.normalize();
}

void Particles::setColorGradient(std::vector<ParticleColorGradientStop> stops){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.colorGradient.stops = stops;
    partAnim.colorGradient.normalize();
}

void Particles::clearColorGradient(){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.colorGradient.stops.clear();
}

void Particles::setColorGradientUseSRGB(bool useSRGB){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.colorGradient.useSRGB = useSRGB;
}

void Particles::setAlphaInitializer(float alpha){
    setAlphaInitializer(alpha, alpha);
}

void Particles::setAlphaInitializer(float minAlpha, float maxAlpha){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.alphaInitializer.minAlpha = minAlpha;
    partAnim.alphaInitializer.maxAlpha = maxAlpha;
}

void Particles::setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha){
    setAlphaModifier(fromTime, toTime, fromAlpha, toAlpha, EaseType::LINEAR);
}

void Particles::setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha, EaseType functionType){
    setAlphaModifier(fromTime, toTime, fromAlpha, toAlpha, Ease(functionType));
}

void Particles::setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.alphaModifier.fromTime = fromTime;
    partAnim.alphaModifier.toTime = toTime;
    partAnim.alphaModifier.fromAlpha = fromAlpha;
    partAnim.alphaModifier.toAlpha = toAlpha;
    partAnim.alphaModifier.function = function;
}

void Particles::setSizeInitializer(float size){
    setSizeInitializer(size, size);
}

void Particles::setSizeInitializer(float minSize, float maxSize){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.sizeInitializer.minSize = minSize;
    partAnim.sizeInitializer.maxSize = maxSize;
}

void Particles::setSizeModifier(float fromTime, float toTime, float fromSize, float toSize){
    setSizeModifier(fromTime, toTime, fromSize, toSize, EaseType::LINEAR);
}

void Particles::setSizeModifier(float fromTime, float toTime, float fromSize, float toSize, EaseType functionType){
    setSizeModifier(fromTime, toTime, fromSize, toSize, Ease(functionType));
}

void Particles::setSizeModifier(float fromTime, float toTime, float fromSize, float toSize, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.sizeModifier.fromTime = fromTime;
    partAnim.sizeModifier.toTime = toTime;
    partAnim.sizeModifier.fromSize = fromSize;
    partAnim.sizeModifier.toSize = toSize;
    partAnim.sizeModifier.function = function;
}

void Particles::setSpriteIntializer(std::vector<int> frames){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.spriteInitializer.frames = frames;
}

void Particles::setSpriteIntializer(int minFrame, int maxFrame){
    if (minFrame <= maxFrame){
        ParticlesComponent& partAnim = getComponent<ParticlesComponent>();
        partAnim.spriteInitializer.frames.clear();
        for (int i = minFrame; i <= maxFrame; i++){
            partAnim.spriteInitializer.frames.push_back(i);
        }
    }
}

void Particles::setSpriteModifier(float fromTime, float toTime, std::vector<int> frames){
    setSpriteModifier(fromTime, toTime, frames, EaseType::LINEAR);
}

void Particles::setSpriteModifier(float fromTime, float toTime, std::vector<int> frames, EaseType functionType){
    setSpriteModifier(fromTime, toTime, frames, Ease(functionType));
}

void Particles::setSpriteModifier(float fromTime, float toTime, std::vector<int> frames, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.spriteModifier.fromTime = fromTime;
    partAnim.spriteModifier.toTime = toTime;
    partAnim.spriteModifier.frames = frames;
    partAnim.spriteModifier.function = function;
}

void Particles::setRotationInitializer(Quaternion rotation){
    setRotationInitializer(rotation, rotation);
}

void Particles::setRotationInitializer(float rotation){
    setRotationInitializer(rotation, rotation);
}

void Particles::setRotationInitializer(Quaternion minRotation, Quaternion maxRotation){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.rotationInitializer.minRotation = minRotation;
    partAnim.rotationInitializer.maxRotation = maxRotation;
}

void Particles::setRotationInitializer(float minRotation, float maxRotation){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.rotationInitializer.minRotation = Quaternion(0, 0, minRotation);
    partAnim.rotationInitializer.maxRotation = Quaternion(0, 0, maxRotation);
}

void Particles::setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation){
    setRotationModifier(fromTime, toTime, fromRotation, toRotation, EaseType::LINEAR);
}

void Particles::setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation){
    setRotationModifier(fromTime, toTime, fromRotation, toRotation, EaseType::LINEAR);
}

void Particles::setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation, EaseType functionType){
    setRotationModifier(fromTime, toTime, fromRotation, toRotation, Ease(functionType));
}

void Particles::setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.rotationModifier.fromTime = fromTime;
    partAnim.rotationModifier.toTime = toTime;
    partAnim.rotationModifier.fromRotation = Quaternion(0, 0, fromRotation);
    partAnim.rotationModifier.toRotation = Quaternion(0, 0, toRotation);;
    partAnim.rotationModifier.function = function;
}

void Particles::setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation, EaseType functionType){
    setRotationModifier(fromTime, toTime, fromRotation, toRotation, Ease(functionType));
}

void Particles::setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.rotationModifier.fromTime = fromTime;
    partAnim.rotationModifier.toTime = toTime;
    partAnim.rotationModifier.fromRotation = fromRotation;
    partAnim.rotationModifier.toRotation = toRotation;
    partAnim.rotationModifier.function = function;
}

void Particles::setScaleInitializer(float scale){
    setScaleInitializer(scale, scale);
}

void Particles::setScaleInitializer(Vector3 scale){
    setScaleInitializer(scale, scale);
}

void Particles::setScaleInitializer(float minScale, float maxScale){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.scaleInitializer.minScale = Vector3(minScale, minScale, minScale);
    partAnim.scaleInitializer.maxScale = Vector3(maxScale, maxScale, maxScale);
}

void Particles::setScaleInitializer(Vector3 minScale, Vector3 maxScale){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.scaleInitializer.minScale = minScale;
    partAnim.scaleInitializer.maxScale = maxScale;
}

void Particles::setScaleModifier(float fromTime, float toTime, float fromScale, float toScale){
    setScaleModifier(fromTime, toTime, Vector3(fromScale, fromScale, fromScale), Vector3(toScale, toScale, toScale), EaseType::LINEAR);
}

void Particles::setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale){
    setScaleModifier(fromTime, toTime, fromScale, toScale, EaseType::LINEAR);
}

void Particles::setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale, EaseType functionType){
    setScaleModifier(fromTime, toTime, fromScale, toScale, Ease(functionType));
}

void Particles::setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale, Ease function){
    ParticlesComponent& partAnim = getComponent<ParticlesComponent>();

    partAnim.scaleModifier.fromTime = fromTime;
    partAnim.scaleModifier.toTime = toTime;
    partAnim.scaleModifier.fromScale = fromScale;
    partAnim.scaleModifier.toScale = toScale;
    partAnim.scaleModifier.function = function;
}

