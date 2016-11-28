
#include "AudioPlayer.h"

#include "AudioLoader.h"


AudioPlayer::AudioPlayer(){
    isLoaded = false;
}

AudioPlayer::~AudioPlayer(){
    
}

void AudioPlayer::setFile(std::string filename){
    this->filename = filename;
}


