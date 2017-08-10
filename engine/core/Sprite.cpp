#include "Sprite.h"
#include "platform/Log.h"
#include "Engine.h"

using namespace Supernova;

Sprite::Sprite(): Image(){
    inAnimation = false;
}

Sprite::~Sprite(){

}

void Sprite::addFrame(std::string id, float x, float y, float width, float height){
    framesRect[id] = Rect(x, y, width, height);
}

void Sprite::removeFrame(std::string id){
    framesRect.erase(id);
}

void Sprite::setFrame(std::string id){
    if (framesRect.count(id)){
        setTectureRect(framesRect[id]);
    }
}

void Sprite::setFrame(int id){
    std::unordered_map<std::string, Rect>::iterator it = framesRect.begin();
    if (id >= 0 && id < framesRect.size()){
        std::advance(it,(framesRect.size()-id-1));
        setTectureRect(it->second);
    }
}

void Sprite::animate(std::vector<int> framesTime, std::vector<int> frames, bool loop){
    inAnimation = true;

    if (framesTime.size() == 0){
        inAnimation = false;
        Log::Error(LOG_TAG, "Incorrect: no framesTime");
    }else if (frames.size() == 0){
        inAnimation = false;
        Log::Error(LOG_TAG, "Incorrect: no frames");
    }

    animation.frames = frames;
    animation.framesTime = framesTime;
    animation.loop = loop;
    animation.framesIndex = 0;
    animation.framesTimeIndex = framesTime.size()-1; //Duration time
    animation.timecount = 0;

    if (inAnimation) {
        setFrame(animation.frames[animation.framesIndex]);
    }
}

void Sprite::animate(std::vector<int> framesTime, int startFrame, int endFrame, bool loop){
    
    inAnimation = true;
    
    if (startFrame < 0 && startFrame >= framesRect.size()){
        inAnimation = false;
        Log::Error(LOG_TAG, "Incorrect range of startFrame");
    }else if (endFrame < 0 && endFrame >= framesRect.size()){
        inAnimation = false;
        Log::Error(LOG_TAG, "Incorrect range of endFrame");
    }
    
    if (inAnimation){
        std::vector<int> frames;
        int actualFrame = startFrame;
        bool finaliza = false;
        while (!finaliza){

            if (actualFrame >= framesRect.size())
                actualFrame = 0;

            frames.push_back(actualFrame);

            if (actualFrame == endFrame)
                finaliza = true;

            actualFrame++;
        }

        animate(framesTime, frames, loop);
    }
}

void Sprite::animate(int interval, int startFrame, int endFrame, bool loop){
    inAnimation = true;

    if (interval <= 0) {
        inAnimation = false;
        Log::Error(LOG_TAG, "Incorrect interval");
    }

    if (inAnimation) {
        std::vector<int> framesTime;
        framesTime.push_back(interval);

        animate(framesTime, startFrame, endFrame, loop);
    }
}

bool Sprite::draw(){
    
    if (inAnimation){
        animation.timecount += Engine::getFrametime();
        while ((animation.timecount >= animation.framesTime[animation.framesTimeIndex]) && (inAnimation)){

            animation.timecount -= animation.framesTime[animation.framesTimeIndex];

            setFrame(animation.frames[animation.framesIndex]);

            animation.framesIndex++;
            animation.framesTimeIndex++;

            if (animation.framesIndex == animation.frames.size()-1){
                if (!animation.loop){
                    inAnimation = false;
                }
            }

            if (animation.framesIndex >= animation.frames.size())
                animation.framesIndex = 0;

            if (animation.framesTimeIndex >= animation.framesTime.size())
                animation.framesTimeIndex = 0;
        
        }
    }
    
    return Image::draw();
}
