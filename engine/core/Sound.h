

#ifndef Sound_h
#define Sound_h

#include "audio/AudioPlayer.h"

class Sound{
    
private:

    AudioPlayer* player;
    
public:
    Sound(const char* filename);
    virtual ~Sound();

    int load();
    void destroy();

    int play();
    int stop();

};

#endif /* Sound_h */
