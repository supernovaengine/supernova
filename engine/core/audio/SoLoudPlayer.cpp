
#include "SoLoudPlayer.h"
#include "file/Data.h"
#include "soloud_thread.h"
#include "SoundManager.h"
#include "Log.h"

using namespace Supernova;

SoLoudPlayer::SoLoudPlayer(): AudioPlayer(){
}

SoLoudPlayer::~SoLoudPlayer(){
    destroy();
}

int SoLoudPlayer::load(){
    Data file(filename.c_str());

    SoLoud::result res = sample.loadMem(file.getMemPtr(), file.length(), false, false);

    if (res == SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED){
        Log::Error("Audio file type of '%s' could not be loaded", filename.c_str());
        return res;
    }else if (res == SoLoud::SOLOUD_ERRORS::OUT_OF_MEMORY){
        Log::Error("Out of memory when loading '%s'", filename.c_str());
        return res;
    }else if (res == SoLoud::SOLOUD_ERRORS::UNKNOWN_ERROR){
        Log::Error("Unknown error when loading '%s'", filename.c_str());
        return res;
    }

    sample.setSingleInstance(true);
    sample.setVolume(1.0);

    loaded = true;

    return res;

}

void SoLoudPlayer::destroy(){
    loaded = false;
}

int SoLoudPlayer::play(){
    if (!loaded) {
        SoLoud::result res = load();

        if (res != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR)
            return res;
    }

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
