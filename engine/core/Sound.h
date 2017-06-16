

#ifndef Sound_h
#define Sound_h

#include "audio/AudioPlayer.h"

#include <string>

namespace Supernova {

    class Sound{

    private:
        AudioPlayer* player;
        std::string filename;

    public:
        Sound(std::string filename);
        virtual ~Sound();

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

#endif /* Sound_h */
