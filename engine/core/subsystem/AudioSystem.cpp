//
// (c) 2022 Eduardo Doria.
//

#include "AudioSystem.h"

#include "Scene.h"

#include "io/Data.h"
#include "soloud.h"
#include "soloud_thread.h"
#include "soloud_wav.h"

using namespace Supernova;

SoLoud::Soloud AudioSystem::soloud;
bool AudioSystem::inited = false;

AudioSystem::AudioSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<AudioComponent>());

    cameraLastPosition = Vector3(0, 0, 0);
}

void AudioSystem::init(){
    if (!inited) {
        soloud.init();

        //Wait for mixing thread
        //SoLoud::Thread::sleep(10);

        inited = true;
    }
}

void AudioSystem::deInit(){
    if (inited){
        soloud.deinit();

        inited = false;
    }
}

bool AudioSystem::loadAudio(AudioComponent& audio, Entity entity){
    Data filedata;

    if (filedata.open(audio.filename.c_str()) != FileErrors::FILEDATA_OK){
        Log::error("Audio file not found: %s", audio.filename.c_str());
        return false;
    }

    SoLoud::result res = samples[entity].loadMem(filedata.getMemPtr(), filedata.length(), false, false);

    if (res == SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED){
        Log::error("Audio file type of '%s' could not be loaded", audio.filename.c_str());
        return false;
    }else if (res == SoLoud::SOLOUD_ERRORS::OUT_OF_MEMORY){
        Log::error("Out of memory when loading '%s'", audio.filename.c_str());
        return false;
    }else if (res == SoLoud::SOLOUD_ERRORS::UNKNOWN_ERROR){
        Log::error("Unknown error when loading '%s'", audio.filename.c_str());
        return false;
    }

    samples[entity].setSingleInstance(true);
    samples[entity].setVolume(1.0);

    audio.length = samples[entity].getLength();

    audio.loaded = true;

    return true;
}

void AudioSystem::destroyAudio(AudioComponent& audio){
    audio.loaded = false;
}

bool AudioSystem::seekAudio(AudioComponent& audio, double time){
    SoLoud::result res = soloud.seek(audio.handle, time);

    if (res != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR)
        return false;

    return true;
}

void AudioSystem::stopAll(){
    if (inited){
        soloud.stopAll();
    }
}

void AudioSystem::pauseAll(){
    if (inited) {
        soloud.setPauseAll(true);
    }
}

void AudioSystem::resumeAll(){
    if (inited) {
        soloud.setPauseAll(false);
    }
}

void AudioSystem::checkActive(){
    if (inited) {
        if (soloud.getVoiceCount() == 0){
            deInit();
        }
    }
}

void AudioSystem::load(){
}

void AudioSystem::destroy(){
}

void AudioSystem::update(double dt){
    auto audios = scene->getComponentArray<AudioComponent>();
    for (int i = 0; i < audios->size(); i++){
		AudioComponent& audio = audios->getComponentFromIndex(i);

        Entity entity = audios->getEntity(i);
        Signature signature = scene->getSignature(entity);

        Vector3 worldPosition = Vector3(0, 0, 0);
        if (signature.test(scene->getComponentType<Transform>()) && audio.enable3D){
            Transform& transform = scene->getComponent<Transform>(entity);

            worldPosition = transform.worldPosition;
        }

        if (audio.state == AudioState::Playing || audio.startTrigger){
            if (!audio.loaded){
                loadAudio(audio, entity);
            }
        }

        if (audio.loaded){
            if (audio.startTrigger){
                audio.startTrigger = false;

                if (audio.state != AudioState::Paused) {
                    init();
                    if (audio.enable3D){
                        if (audio.enableClocked){
                            audio.handle = soloud.play3dClocked(Engine::getDeltatime(), samples[entity], worldPosition.x, worldPosition.y, worldPosition.z);
                        }else{
                            audio.handle = soloud.play3d(samples[entity], worldPosition.x, worldPosition.y, worldPosition.z);
                        }
                    }else{
                        if (audio.enableClocked){
                            audio.handle = soloud.playClocked(Engine::getDeltatime(), samples[entity]);
                        }else{
                            audio.handle = soloud.play(samples[entity]);
                        }
                    }
                }else{
                    soloud.setPause(audio.handle, false);
                }
                if (audio.enable3D){
                    Transform& cameraTransform =  scene->getComponent<Transform>(scene->getCamera());

                    audio.lastPosition = worldPosition;
                    cameraLastPosition = cameraTransform.worldPosition;
                }

                audio.state = AudioState::Playing;
            }
            if (audio.pauseTrigger){
                audio.pauseTrigger = false;

                soloud.setPause(audio.handle, true);
                audio.state = AudioState::Paused;
            }
            if (audio.stopTrigger){
                audio.stopTrigger = false;

                soloud.stop(audio.handle);
                audio.state = AudioState::Stopped;
            }

            if (audio.state == AudioState::Playing){
                audio.playingTime = soloud.getStreamTime(audio.handle);

                if (audio.needUpdate){
                    soloud.setVolume(audio.handle, audio.volume);
                    soloud.setRelativePlaySpeed(audio.handle, audio.speed);
                    soloud.setPan(audio.handle, audio.pan);
                    soloud.setLooping(audio.handle, audio.looping);
                    soloud.setLoopPoint(audio.handle, audio.loopingPoint);
                    soloud.setProtectVoice(audio.handle, audio.protectVoice);
                    soloud.setInaudibleBehavior(audio.handle, audio.inaudibleBehaviorMustTick, audio.inaudibleBehaviorKill);

                    if (audio.enable3D){
                        CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
                        Transform& cameraTransform =  scene->getComponent<Transform>(scene->getCamera());

                        Vector3 velocity = audio.lastPosition - worldPosition;

                        Vector3 camWorldPos = cameraTransform.worldPosition;
                        Vector3 camWorldView = camera.worldView - camWorldPos;
                        Vector3 camWorldUp = camera.worldUp;
                        Vector3 camVelocity = cameraLastPosition - camWorldPos;

                        unsigned int attModel = SoLoud::AudioSource::NO_ATTENUATION;
                        if (audio.attenuationModel == Audio3DAttenuation::INVERSE_DISTANCE)
                            attModel = SoLoud::AudioSource::INVERSE_DISTANCE;
                        if (audio.attenuationModel == Audio3DAttenuation::LINEAR_DISTANCE)
                            attModel = SoLoud::AudioSource::LINEAR_DISTANCE;
                        if (audio.attenuationModel == Audio3DAttenuation::EXPONENTIAL_DISTANCE)
                            attModel = SoLoud::AudioSource::EXPONENTIAL_DISTANCE;

                        soloud.set3dSourceMinMaxDistance(audio.handle, audio.minDistance, audio.maxDistance);
                        soloud.set3dSourceAttenuation(audio.handle, attModel, audio.attenuationRolloffFactor);
                        soloud.set3dSourceDopplerFactor(audio.handle, audio.dopplerFactor);
                        soloud.set3dSourceParameters(
                            audio.handle, 
                            worldPosition.x, worldPosition.y, worldPosition.z, 
                            velocity.x, velocity.y, velocity.z);
                        soloud.set3dListenerParameters(
                            camWorldPos.x, camWorldPos.y, camWorldPos.z, 
                            camWorldView.x, camWorldView.y, camWorldView.z, 
                            camWorldUp.x, camWorldUp.y, camWorldUp.z,
                            camVelocity.x, camVelocity.y, camVelocity.z);

                        soloud.update3dAudio();

                        audio.lastPosition = worldPosition;
                        cameraLastPosition = camWorldPos;
                    }

                    audio.needUpdate = false;
                }
            }
        }
    }
}

void AudioSystem::draw(){

}

void AudioSystem::entityDestroyed(Entity entity){

}