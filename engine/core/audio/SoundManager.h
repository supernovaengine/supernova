
#ifndef SoundManager_h
#define SoundManager_h

#include "AudioPlayer.h"
#include <vector>
#include <string>

class SoundManager {
    
    typedef struct {
        AudioPlayer* player;
        std::string key;
    } PlayerStore;
    
private:
    
    static std::vector<PlayerStore> players;
    
public:
    
    AudioPlayer* loadPlayer(std::string relative_path);
    void deletePlayer(std::string relative_path);
    
    static void stopAll();
    static void clear();
};

#endif /* SoundManager_h */
