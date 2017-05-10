
#include "Sound.h"
#include "SoLoudPlayer.h"
#include "platform/Log.h"

Sound::Sound(std::string filename){
    this->filename = filename;
    player = soundManager.loadPlayer(filename);
}

Sound::~Sound(){
    destroy();
}

int Sound::load(){
    return player->load();
}

void Sound::destroy(){
    soundManager.deletePlayer(filename);
}

int Sound::play(){
    return player->play();
}

int Sound::stop(){
    return player->stop();
}


