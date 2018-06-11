#include "SpriteAnimation.h"
#include "Sprite.h"
#include "Log.h"

using namespace Supernova;

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop): Action(){
    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = 0;

    this->loop = loop;
    
    this->framesTime = framesTime;
    this->frames = frames;
    
    this->startFrame = 0;
    this->endFrame = 0;
}

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop): Action(){
    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = 0;

    this->loop = loop;

    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    
}

SpriteAnimation::SpriteAnimation(int interval, int startFrame, int endFrame, bool loop): Action(){
    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = 0;

    this->loop = loop;
    
    std::vector<int> framesTime;
    framesTime.push_back(interval);
    
    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
}

SpriteAnimation::SpriteAnimation(int interval, std::vector<int> frames, bool loop): Action(){
    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = 0;

    this->loop = loop;
    
    std::vector<int> framesTime;
    framesTime.push_back(interval);

    this->framesTime = framesTime;
    this->frames = frames;

    this->startFrame = 0;
    this->endFrame = 0;
}

SpriteAnimation::~SpriteAnimation(){
    
}

bool SpriteAnimation::run(){
    if (!Action::run())
        return false;

    if (Sprite* sprite = dynamic_cast<Sprite*>(object)) {

        bool erro = false;

        if (framesTime.size() == 0){
            Log::Error("Incorrect sprite animation: no framesTime");
            erro = true;
        }else if (frames.size() == 0 && startFrame == 0 && endFrame == 0){
            Log::Error("Incorrect sprite animation: no frames");
            erro = true;
        }else if (startFrame < 0 && startFrame >= sprite->getFramesSize()){
            Log::Error("Incorrect sprite animation: range of startFrame");
            erro = true;
        }else if (endFrame < 0 && endFrame >= sprite->getFramesSize()){
            Log::Error("Incorrect sprite animation: range of endFrame");
            erro = true;
        }

        if (!erro) {
            if (startFrame > 0 || endFrame > 0) {
                std::vector<int> frames;
                int actualFrame = startFrame;
                bool finaliza = false;
                while (!finaliza) {

                    if (actualFrame >= sprite->getFramesSize())
                        actualFrame = 0;

                    frames.push_back(actualFrame);

                    if (actualFrame == endFrame)
                        finaliza = true;

                    actualFrame++;
                }

                this->frames = frames;
            }

            sprite->setFrame(frames[framesIndex]);
        }else{
            Log::Error("Object in SpriteAnimation must be a Sprite type");
            stop();
        }
    }
    
    return true;
}

bool SpriteAnimation::pause(){
    return Action::pause();
}

bool SpriteAnimation::stop(){
    if (!Action::stop())
        return false;

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = 0;
    
    return true;
}

bool SpriteAnimation::step(){
    if (!Action::step())
        return false;
    
    if (Sprite* sprite = dynamic_cast<Sprite*>(object)){

        spriteFrameCount += steptime;
        while ((spriteFrameCount >= framesTime[framesTimeIndex]) && (isRunning())) {

            spriteFrameCount -= framesTime[framesTimeIndex];
            
            framesIndex++;
            framesTimeIndex++;
            
            if (framesIndex == frames.size() - 1) {
                if (!loop) {
                    stop();
                    onFinish.call(object);
                }
            }
            
            if (framesIndex >= frames.size())
                framesIndex = 0;
            
            if (framesTimeIndex >= framesTime.size())
                framesTimeIndex = 0;

            sprite->setFrame(frames[framesIndex]);

        }

    }
    
    return true;
}
