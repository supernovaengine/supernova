

#ifndef Sound_h
#define Sound_h

#include "audio/SoundManager.h"
#include "audio/AudioPlayer.h"
#include "soloud.h"
#include "audio/SoLoudSource.h"
#include "soloud_thread.h"

#include <string>

class Sound{

private:
    SoundManager soundManager;
    AudioPlayer* player;
    std::string filename;

public:
    Sound(std::string filename);
    virtual ~Sound();

    int load();
    void destroy();

    int play();
    int stop();

};

#endif /* Sound_h */
