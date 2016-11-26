
#include "Sound.h"

#include "audio/OpenalPlayer.h"

Sound::Sound(const char* filename){
    player = new OpenalPlayer();
    player->setFile(filename);
}

Sound::~Sound(){
    delete player;
}

int Sound::load(){
    
    return player->load();
    
}

int Sound::play(){

    return player->play();

}

int Sound::stop(){
    
    return player->stop();
    
}

void Sound::destroy(){
    
    player->destroy();
    
}
