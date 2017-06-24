
#include "AudioPlayer.h"

#include "AudioLoader.h"

using namespace Supernova;

AudioPlayer::AudioPlayer(){
    filename = "";
    loaded = false;
    state = 0;
    audioFile = NULL;
}

AudioPlayer::~AudioPlayer(){
    
}

void AudioPlayer::setFile(std::string filename){
    this->filename = filename;
}


