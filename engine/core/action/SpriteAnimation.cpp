#include "SpriteAnimation.h"
#include "Sprite.h"
#include "Engine.h"
#include "platform/Log.h"

using namespace Supernova;

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop): Action(-1, loop){
    this->function = NULL;
    this->functionLua = 0;
    
    this->framesTime = framesTime;
    this->frames = frames;
    
    this->startFrame = 0;
    this->endFrame = 0;
}

SpriteAnimation::SpriteAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop): Action(-1, loop){
    this->function = NULL;
    this->functionLua = 0;

    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    
}

SpriteAnimation::SpriteAnimation(int interval, int startFrame, int endFrame, bool loop): Action(-1, loop){
    std::vector<int> framesTime;
    framesTime.push_back(interval);
    
    this->function = NULL;
    this->functionLua = 0;
    
    this->framesTime = framesTime;
    
    this->startFrame = startFrame;
    this->endFrame = endFrame;
}

SpriteAnimation::~SpriteAnimation(){
    
}

void SpriteAnimation::play(){
    Action::play();
    
    if (object){
        if (startFrame > 0 || endFrame > 0){
            ((Sprite*)object)->animate(framesTime, startFrame, endFrame, loop);
        }else{
            ((Sprite*)object)->animate(framesTime, frames, loop);
        }
    }
}

void SpriteAnimation::stop(){
    Action::stop();
    
    if (object){
        if (((Sprite*)object)->isInAnimation()){
            ((Sprite*)object)->stop();
        }
    }
}

void SpriteAnimation::step(){
    Action::step();
    
    if (object){
        if (!((Sprite*)object)->isInAnimation()){
            stop();
        }
    }
}
