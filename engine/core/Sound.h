

#ifndef Sound_h
#define Sound_h

#include "audio/SoundManager.h"
#include "audio/AudioPlayer.h"
#include "soloud.h"
#include "audio/SoLoudLoader.h"
#include "soloud_thread.h"

#include <string>

class Sound{

private:

    // Define a couple of variables
    SoLoud::Soloud soloud;  // SoLoud engine core
    //SoLoud::Speech speech;  // A sound source (speech, in this case)
    SoLoud::Wav sample;    // One sample
    SoLoud::result loaded;

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
