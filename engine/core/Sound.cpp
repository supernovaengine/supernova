
#include "Sound.h"



Sound::Sound(std::string filename){
    this->filename = filename;
    player = soundManager.loadPlayer(filename);
}

Sound::~Sound(){
    delete player;
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


