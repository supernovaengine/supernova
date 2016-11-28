
#include "SoundManager.h"
#include "OpenalPlayer.h"

std::vector<SoundManager::PlayerStore> SoundManager::players;


AudioPlayer* SoundManager::loadPlayer(std::string relative_path){
    
    for (unsigned i=0; i<players.size(); i++){
        std::string teste = players.at(i).key;
        if (teste == relative_path){
            return players.at(i).player;
        }
    }
    
    AudioPlayer* player = new OpenalPlayer();
    player->setFile(relative_path);
    players.push_back((PlayerStore){player, relative_path});
    return player;
}


void SoundManager::deletePlayer(std::string relative_path){
    int remove = -1;
    
    for (unsigned i=0; i<players.size(); i++){
        if (players.at(i).key == relative_path){
            remove = i;
            players.at(i).player->destroy();
        }
    }
    
    if (remove > 0)
        players.erase(players.begin() + remove);
    
}

void SoundManager::stopAll(){
    for (unsigned i=0; i<players.size(); i++){
        players.at(i).player->stop();
    }
}

void SoundManager::clear(){
    players.clear();
}
