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
        setRect(framesRect[id]);
    }
}

void Sprite::setFrame(int id){
    std::unordered_map<std::string, Rect>::iterator it = framesRect.begin();
    if (id >= 0 && id < framesRect.size()){
        std::advance(it,(framesRect.size()-id-1));
        setRect(it->second);
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
        animation.framesTime = framesTime;
        animation.startFrame = startFrame;
        animation.endFrame = endFrame;
        animation.loop = loop;
        animation.actualFrame = startFrame;
        animation.actualFramesTime = 0;
        animation.timecount = 0;
        
        setFrame(animation.actualFrame);
    }

}

bool Sprite::draw(){
    
    if (inAnimation){
        animation.timecount += Engine::getFrametime();
        while ((animation.timecount >= animation.framesTime[animation.actualFramesTime]) && (inAnimation)){
            
            animation.timecount -= animation.framesTime[animation.actualFramesTime];
            
            animation.actualFrame++;
            animation.actualFramesTime++;
            
            if (animation.actualFrame == animation.endFrame){
                if (!animation.loop){
                    inAnimation = false;
                }else{
                    animation.actualFrame = animation.startFrame;
                }
            }
            
            if (animation.actualFrame >= framesRect.size())
                animation.actualFrame = 0;
            
            if (animation.actualFramesTime >= animation.framesTime.size())
                animation.actualFramesTime = 0;
            
            setFrame(animation.actualFrame);
        
        }
    }
    
    return Image::draw();
}
