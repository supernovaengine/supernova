
#include "Sound.h"
#include "SoLoudPlayer.h"
#include "platform/Log.h"

using namespace Supernova;

Sound::Sound(std::string filename){
    this->filename = filename;
    player = new SoLoudPlayer();
    player->setFile(filename);
}

Sound::~Sound(){
    delete player;
}

int Sound::load(){
    return player->load();
}

void Sound::destroy(){
}

int Sound::play(){
    return player->play();
}

int Sound::pause(){
    return player->pause();
}

int Sound::stop(){
    return player->stop();
}

double Sound::getLength(){
    return player->getLength();
}

double Sound::getPlayingTime(){
    return player->getPlayingTime();
}

bool Sound::isPlaying(){
    return player->isPlaying();
}

bool Sound::isPaused(){
    return player->isPaused();
}

bool Sound::isStopped(){
    return player->isStopped();
}


