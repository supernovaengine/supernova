
#include "Sound.h"
#include "platform/Log.h"

Sound::Sound(std::string filename){
    this->filename = filename;
    //player = soundManager.loadPlayer(filename);

}

Sound::~Sound(){
    //delete player;
}

int Sound::load(){

    loaded = sample.load(filename.c_str());
    sample.setSingleInstance(true);
    soloud.init();
    //Wait for mixing thread
    SoLoud::Thread::sleep(10);

    //return player->load();

    return 0;
    
}

void Sound::destroy(){
    
    //soundManager.deletePlayer(filename);
    
}

int Sound::play(){
    
    soloud.play(sample);
/*
    // Wait until sounds have finished
    while (soloud.getActiveVoiceCount() > 0)
    {
        // Still going, sleep for a bit
        SoLoud::Thread::sleep(100);
    }

    // Clean up SoLoud
    soloud.deinit();
*/
    return 0;


    //return player->play();

}

int Sound::stop(){
    
    //return player->stop();

    return 0;
    
}


