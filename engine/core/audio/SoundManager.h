
#ifndef SoundManager_h
#define SoundManager_h

#include "AudioFile.h"
#include "soloud.h"

class SoundManager {
private:
    static SoLoud::Soloud soloud;

    static bool inited;
    
public:

    static SoLoud::Soloud* init();
    static void deInit();

    static void checkActive();

    static void stopAll();
    static void pauseAll();
    static void resumeAll();
};

#endif /* SoundManager_h */
