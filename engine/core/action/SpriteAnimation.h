#ifndef SpriteAnimation_h
#define SpriteAnimation_h

#include "Action.h"
#include <vector>

namespace Supernova{
    
    class SpriteAnimation: public Action{
        
    protected:
        bool loop;
        std::vector<int> framesTime;
        std::vector<int> frames;
        int framesIndex;
        int framesTimeIndex;
        unsigned int spriteFrameCount;
        
        int startFrame;
        int endFrame;
        
    public:
        SpriteAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop);
        SpriteAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        SpriteAnimation(int interval, int startFrame, int endFrame, bool loop);
        SpriteAnimation(int interval, std::vector<int> frames, bool loop);
        virtual ~SpriteAnimation();
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool update(float interval);
    };
}

#endif /* SpriteAnimation_h */
