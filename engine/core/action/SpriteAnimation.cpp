#include "SpriteAnimation.h"
#include "Sprite.h"
#include "platform/Log.h"

using namespace Supernova;

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop): Action(-1, loop){
    this->function = NULL;
    this->functionLua = 0;

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = (int)framesTime.size()-1; //Actual sprite frame with the last frameTime

    this->framesTime = framesTime;
    this->frames = frames;
    
    this->startFrame = 0;
    this->endFrame = 0;
}

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop): Action(-1, loop){
    this->function = NULL;
    this->functionLua = 0;

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = (int)framesTime.size()-1; //Actual sprite frame with the last frameTime

    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    
}

SpriteAnimation::SpriteAnimation(int interval, int startFrame, int endFrame, bool loop): Action(-1, loop){
    std::vector<int> framesTime;
    framesTime.push_back(interval);
    
    this->function = NULL;
    this->functionLua = 0;

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = (int)framesTime.size()-1; //Actual sprite frame with the last frameTime
    
    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
}

SpriteAnimation::SpriteAnimation(int interval, std::vector<int> frames, bool loop): Action(-1, loop){
    std::vector<int> framesTime;
    framesTime.push_back(interval);

    this->function = NULL;
    this->functionLua = 0;

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = (int)framesTime.size()-1; //Actual sprite frame with the last frameTime

    this->framesTime = framesTime;
    this->frames = frames;

    this->startFrame = 0;
    this->endFrame = 0;
}

SpriteAnimation::~SpriteAnimation(){
    
}

void SpriteAnimation::play(){
    Action::play();

    if (object) {

        bool erro = false;

        if (framesTime.size() == 0){
            Log::Error(LOG_TAG, "Incorrect sprite animation: no framesTime");
            erro = true;
        }else if (frames.size() == 0 && startFrame == 0 && endFrame == 0){
            Log::Error(LOG_TAG, "Incorrect sprite animation: no frames");
            erro = true;
        }else if (startFrame < 0 && startFrame >= ((Sprite *) object)->getFramesSize()){
            Log::Error(LOG_TAG, "Incorrect sprite animation: range of startFrame");
            erro = true;
        }else if (endFrame < 0 && endFrame >= ((Sprite *) object)->getFramesSize()){
            Log::Error(LOG_TAG, "Incorrect sprite animation: range of endFrame");
            erro = true;
        }

        if (!erro) {
            if (startFrame > 0 || endFrame > 0) {
                std::vector<int> frames;
                int actualFrame = startFrame;
                bool finaliza = false;
                while (!finaliza) {

                    if (actualFrame >= ((Sprite *) object)->getFramesSize())
                        actualFrame = 0;

                    frames.push_back(actualFrame);

                    if (actualFrame == endFrame)
                        finaliza = true;

                    actualFrame++;
                }

                this->frames = frames;
            }

            ((Sprite *) object)->setFrame(frames[framesIndex]);
        }else{
            stop();
        }
    }
}

void SpriteAnimation::stop(){
    Action::stop();
}

void SpriteAnimation::reset(){
    Action::reset();

    this->spriteFrameCount = 0;
    this->framesIndex = 0;
    this->framesTimeIndex = (int)framesTime.size()-1; //Duration time
}

void SpriteAnimation::step(){
    Action::step();
    
    if (object){

        spriteFrameCount += steptime;
        while ((spriteFrameCount >= framesTime[framesTimeIndex]) && (isRunning())) {

            spriteFrameCount -= framesTime[framesTimeIndex];

            ((Sprite*)object)->setFrame(frames[framesIndex]);

            framesIndex++;
            framesTimeIndex++;

            if (framesIndex == frames.size() - 1) {
                if (!loop) {
                    stop();
                }
            }

            if (framesIndex >= frames.size())
                framesIndex = 0;

            if (framesTimeIndex >= framesTime.size())
                framesTimeIndex = 0;

        }

    }
}
