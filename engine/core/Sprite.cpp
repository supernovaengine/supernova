#include "Sprite.h"
#include "platform/Log.h"
#include "Engine.h"

using namespace Supernova;

Sprite::Sprite(): Image(){
    inAnimation = false;
    animationFrame = 0;
    animationAcc = 0;
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
        std::advance(it,id);
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
        
        animationFrame = startFrame;
        
        setFrame(animationFrame);
    }

    int duration = 0;
    for(std::vector<int>::iterator it = framesTime.begin(); it != framesTime.end(); ++it)
        duration += *it;

    this->duration = (float)duration / 1000;
    this->loop = loop;
    this->function = S_TIMELINE_LINEAR;

    this->play(this);
}

void Sprite::step(){
    Timeline::step();

    Log::Debug(LOG_TAG, "sprite step time %f value %f \n", time, value);
}

bool Sprite::draw(){
    /*
    if (inAnimation){
        animationAcc += Engine::getFrametime();
        while (animationAcc >= 100){
            
            animationFrame += 1;
            if (animationFrame >= framesRect.size() || animationFrame > animation.endFrame)
                animationFrame = animation.startFrame;
            
            setFrame(animationFrame);
            
            animationAcc -= 100;
        }
    }
    */
    return Image::draw();
}
