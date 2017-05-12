
#include "SoLoudPlayer.h"
#include "AudioLoader.h"
#include "soloud_thread.h"

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

    soloud.init();

    //Wait for mixing thread
    SoLoud::Thread::sleep(10);

    return 0;

}

int SoLoudPlayer::play(){
    soloud.play(sample);
    return 0;
}

int SoLoudPlayer::pause(){
    return 0;
}

int SoLoudPlayer::resume(){
    return 0;
}

int SoLoudPlayer::stop(){
    return 0;
}

void SoLoudPlayer::destroy(){
    delete audioFile;
}