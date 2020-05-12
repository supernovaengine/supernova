#ifndef AudioPlayer_h
#define AudioPlayer_h

#include <string>

#define S_AUDIO_PLAYING 1
#define S_AUDIO_STOPED 2
#define S_AUDIO_PAUSED 3

namespace Supernova {

    class AudioPlayer{
        
    protected:
        std::string filename;
        bool loaded;
        int state;
    public:
        AudioPlayer();
        virtual ~AudioPlayer();
        
        void setFile(std::string filename);
        
        virtual int load() = 0;
        virtual void destroy() = 0;

        virtual int play() = 0;
        virtual int pause() = 0;
        virtual int stop() = 0;

        virtual double getLength() = 0;
        virtual double getPlayingTime() = 0;

        virtual bool isPlaying() = 0;
        virtual bool isPaused() = 0;
        virtual bool isStopped() = 0;
        
    };
    
}

#endif
