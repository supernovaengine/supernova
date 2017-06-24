
#include "SoLoudPlayer.h"
#include "AudioLoader.h"
#include "soloud_thread.h"
#include "SoundManager.h"
#include "platform/Log.h"

using namespace Supernova;

SoLoudPlayer::SoLoudPlayer(): AudioPlayer(){
}

SoLoudPlayer::~SoLoudPlayer(){
    destroy();
}

int SoLoudPlayer::load(){
    AudioLoader audioLoader;
    audioFile = audioLoader.loadAudio(filename.c_str());

    sample.load(audioFile);
    sample.setSingleInstance(true);
    sample.setVolume(1.0);

    loaded = true;

    return 0;

}

void SoLoudPlayer::destroy(){
    loaded = false;
    delete audioFile;
}

int SoLoudPlayer::play(){
    if (!loaded)
        load();

    if (state != S_AUDIO_PAUSED) {
        soloud = SoundManager::init();
        soundHandle = soloud->play(sample);
    }else{
        soloud->setPause(soundHandle, false);
    }

    state = S_AUDIO_PLAYING;
    return 0;
}

int SoLoudPlayer::pause(){
    if (soloud){
        soloud->setPause(soundHandle, true);
        state = S_AUDIO_PAUSED;
    }
    return 0;
}

int SoLoudPlayer::stop(){
    if (soloud) {
        soloud->stop(soundHandle);
        state = S_AUDIO_STOPED;
    }
    return 0;
}

double SoLoudPlayer::getLength(){
    if (loaded)
        return sample.getLength();
    else
        return 0;
}

double SoLoudPlayer::getPlayingTime(){
    if (soloud)
        return soloud->getStreamTime(soundHandle);
    else
        return 0;
}

bool SoLoudPlayer::isPlaying(){
    if ((getLength() > getPlayingTime()) && (getPlayingTime() != 0) && (state == S_AUDIO_PLAYING)) {
        return true;
    }
    return false;
}

bool SoLoudPlayer::isPaused(){
    return (state == S_AUDIO_PAUSED);
}

bool SoLoudPlayer::isStopped(){
    return (state == S_AUDIO_STOPED);
}
