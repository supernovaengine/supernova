// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Sound.h"
#include "subsystem/AudioSystem.h"

#include "Log.h"

#include <stdexcept>


using namespace doriax;

Sound::Sound(Scene* scene): EntityHandle(scene){
    addComponent<SoundComponent>();
}

Sound::Sound(Scene* scene, bool is3D): EntityHandle(scene){
    addComponent<SoundComponent>();
    if (is3D){
        addComponent<Transform>();
    }
}

Sound::Sound(Scene* scene, Entity entity): EntityHandle(scene, entity){
}


Sound::~Sound(){

}

Object Sound::getObject() const{
    if (!isSound3D()){
        Log::error("Cannot get object from 2D sound: enable 3D sound first");
        throw std::runtime_error("Cannot get object from 2D sound");
    }

    return Object(scene, entity);
}

int Sound::loadSound(const std::string& filename){
    SoundComponent& audio = getComponent<SoundComponent>();

    audio.filename = filename;

    if (Engine::isViewLoaded())
        return scene->getSystem<AudioSystem>()->loadSound(audio);
    else
        return false;
}

void Sound::destroySound(){
    SoundComponent& audio = getComponent<SoundComponent>();

    scene->getSystem<AudioSystem>()->destroySound(audio);
}

void Sound::play(){
    SoundComponent& audio = getComponent<SoundComponent>();

    audio.startTrigger = true;
    audio.pauseTrigger = false;
    audio.stopTrigger = false;
}

void Sound::pause(){
    SoundComponent& audio = getComponent<SoundComponent>();

    audio.startTrigger = false;
    audio.pauseTrigger = true;
    audio.stopTrigger = false;
}

void Sound::stop(){
    SoundComponent& audio = getComponent<SoundComponent>();

    audio.startTrigger = false;
    audio.pauseTrigger = false;
    audio.stopTrigger = true;
}

void Sound::seek(double time){
    SoundComponent& audio = getComponent<SoundComponent>();

    scene->getSystem<AudioSystem>()->seekSound(audio, time);
}

double Sound::getLength(){
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.length;
}

double Sound::getPlayingTime(){
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.playingTime;
}

bool Sound::isPlaying(){
    SoundComponent& audio = getComponent<SoundComponent>();

    if ((audio.length > audio.playingTime) && (audio.playingTime != 0) && (audio.state == SoundState::Playing)) {
        return true;
    }
    return false;
}

bool Sound::isPaused(){
    SoundComponent& audio = getComponent<SoundComponent>();

    return (audio.state == SoundState::Paused);
}

bool Sound::isStopped(){
    SoundComponent& audio = getComponent<SoundComponent>();

    return (audio.state == SoundState::Stopped);
}

void Sound::setSound3D(bool sound3D){
    bool hasTransform = scene->findComponent<Transform>(entity) != nullptr;
    if (sound3D && !hasTransform){
        addComponent<Transform>();
    }else if (!sound3D && hasTransform){
        removeComponent<Transform>();
    }

    SoundComponent& audio = getComponent<SoundComponent>();
    audio.needUpdate = true;
}

bool Sound::isSound3D() const{
    return scene->findComponent<Transform>(entity) != nullptr;
}

void Sound::setClockedSound(bool enableClocked){
    SoundComponent& audio = getComponent<SoundComponent>();

    audio.enableClocked = enableClocked;
}

bool Sound::isClockedSound() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.enableClocked;
}

void Sound::setVolume(double volume){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.volume != volume){
        audio.volume = volume;
        audio.needUpdate = true;
    }
}

double Sound::getVolume() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.volume;
}

void Sound::setSpeed(float speed){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.speed != speed){
        audio.speed = speed;
        audio.needUpdate = true;
    }
}

float Sound::getSpeed() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.speed;
}

void Sound::setPan(float pan){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.pan != pan){
        audio.pan = pan;
        audio.needUpdate = true;
    }
}

float Sound::getPan() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.pan;
}

void Sound::setLooping(bool looping){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.looping != looping){
        audio.looping = looping;
        audio.needUpdate = true;
    }
}

bool Sound::isLooping() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.looping;
}

void Sound::setLoopingPoint(double loopingPoint){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.loopingPoint != loopingPoint){
        audio.loopingPoint = loopingPoint;
        audio.needUpdate = true;
    }
}

double Sound::getLoopingPoint() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.loopingPoint;
}

void Sound::setProtectVoice(bool protectVoice){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.protectVoice != protectVoice){
        audio.protectVoice = protectVoice;
        audio.needUpdate = true;
    }
}

bool Sound::isProtectVoice() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.protectVoice;
}

void Sound::setInaudibleBehavior(bool mustTick, bool kill){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.inaudibleBehaviorMustTick != mustTick || audio.inaudibleBehaviorKill != kill){
        audio.inaudibleBehaviorMustTick = mustTick;
        audio.inaudibleBehaviorKill = kill;
        audio.needUpdate = true;
    }
}

void Sound::setMinMaxDistance(float minDistance, float maxDistance){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.minDistance != minDistance || audio.maxDistance != maxDistance){
        audio.minDistance = minDistance;
        audio.maxDistance = maxDistance;
        audio.needUpdate = true;
    }
}

void Sound::setMinDistance(float minDistance){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.minDistance != minDistance){
        audio.minDistance = minDistance;
        audio.needUpdate = true;
    }
}

float Sound::getMinDistance() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.minDistance;
}

void Sound::setMaxDistance(float maxDistance){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.maxDistance != maxDistance){
        audio.maxDistance = maxDistance;
        audio.needUpdate = true;
    }
}

float Sound::getMaxDistance() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.maxDistance;
}

void Sound::setAttenuationModel(SoundAttenuation attenuationModel){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.attenuationModel != attenuationModel){
        audio.attenuationModel = attenuationModel;
        audio.needUpdate = true;
    }
}

SoundAttenuation Sound::getAttenuationModel() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.attenuationModel;
}

void Sound::setAttenuationRolloffFactor(float attenuationRolloffFactor){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.attenuationRolloffFactor != attenuationRolloffFactor){
        audio.attenuationRolloffFactor = attenuationRolloffFactor;
        audio.needUpdate = true;
    }
}

float Sound::getAttenuationRolloffFactor() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.attenuationRolloffFactor;
}

void Sound::setDopplerFactor(float dopplerFactor){
    SoundComponent& audio = getComponent<SoundComponent>();

    if (audio.dopplerFactor != dopplerFactor){
        audio.dopplerFactor = dopplerFactor;
        audio.needUpdate = true;
    }
}

float Sound::getDopplerFactor() const{
    SoundComponent& audio = getComponent<SoundComponent>();

    return audio.dopplerFactor;
}