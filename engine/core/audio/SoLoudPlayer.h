
#ifndef SOLOUDPLAYER_H
#define SOLOUDPLAYER_H

#include "AudioPlayer.h"
#include "SoLoudSource.h"
#include "soloud.h"

namespace Supernova {

    class SoLoudPlayer: public AudioPlayer {
    private:
        SoLoud::Soloud* soloud;
        SoLoudSource sample;
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
        double getStreamTime();

        bool isPlaying();
        bool isPaused();
        bool isStopped();
    };
    
}


#endif //SOLOUDPLAYER_H
