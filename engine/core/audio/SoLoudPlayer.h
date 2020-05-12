
#ifndef SOLOUDPLAYER_H
#define SOLOUDPLAYER_H

#include "AudioPlayer.h"
#include "soloud.h"
#include "soloud_wav.h"

namespace Supernova {

    class SoLoudPlayer: public AudioPlayer {
    private:
        SoLoud::Soloud* soloud;
        SoLoud::Wav sample;
        SoLoud::handle soundHandle;
    public:
        SoLoudPlayer();
        virtual ~SoLoudPlayer();

        int load();
        void destroy();

        int play();
        int pause();
        int stop();

        double getLength();
        double getPlayingTime();

        bool isPlaying();
        bool isPaused();
        bool isStopped();
    };
    
}


#endif //SOLOUDPLAYER_H
