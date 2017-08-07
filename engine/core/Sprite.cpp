#include "Sprite.h"
#include "platform/Log.h"
#include "Engine.h"

using namespace Supernova;

Sprite::Sprite(): Image(){

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
    
    if (startFrame >= 0 && startFrame < framesRect.size()){
        Log::Error(LOG_TAG, "Incorrect range of startFrame");
    }else if (endFrame >= 0 && endFrame < framesRect.size()){
        Log::Error(LOG_TAG, "Incorrect range of endFrame");
    }
    
    animation.framesTime = framesTime;
    animation.startFrame = startFrame;
    animation.endFrame = endFrame;
    animation.loop = loop;
    
    //printf("teste %i %i\n", framesTime[0], framesTime[1]);
}

bool Sprite::draw(){
    
    //printf("teste2 %u \n", Engine::getFrameTime());
    
    return Image::draw();
}
