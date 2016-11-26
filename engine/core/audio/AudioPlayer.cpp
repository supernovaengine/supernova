
#include "AudioPlayer.h"

#include "AudioLoader.h"


AudioPlayer::AudioPlayer(){
    isLoaded = false;
}

AudioPlayer::~AudioPlayer(){
    
}

void AudioPlayer::setFile(const char* filename){
    this->filename = filename;
}


