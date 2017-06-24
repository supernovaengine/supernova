
#include "SoundManager.h"

using namespace Supernova;

SoLoud::Soloud SoundManager::soloud;
bool SoundManager::inited = false;

SoLoud::Soloud* SoundManager::init(){
    if (!inited) {
        soloud.init();

        //Wait for mixing thread
        //SoLoud::Thread::sleep(10);

        inited = true;
    }

    return &soloud;
}

void SoundManager::deInit(){
    if (inited){
        soloud.deinit();

        inited = false;
    }
}

void SoundManager::checkActive(){
    if (inited) {
        if (soloud.getVoiceCount() == 0){
            deInit();
        }
    }
}

void SoundManager::stopAll(){
    if (inited){
        soloud.stopAll();
    }
}

void SoundManager::pauseAll(){
    if (inited) {
        soloud.setPauseAll(true);
    }
}

void SoundManager::resumeAll(){
    if (inited) {
        soloud.setPauseAll(false);
    }
}
