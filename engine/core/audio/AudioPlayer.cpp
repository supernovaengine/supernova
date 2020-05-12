
#include "AudioPlayer.h"

using namespace Supernova;

AudioPlayer::AudioPlayer(){
    filename = "";
    loaded = false;
    state = 0;
}

AudioPlayer::~AudioPlayer(){
    
}

void AudioPlayer::setFile(std::string filename){
    this->filename = filename;
}


