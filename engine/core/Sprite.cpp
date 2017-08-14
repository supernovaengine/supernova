#include "Sprite.h"
#include "platform/Log.h"
#include "Engine.h"

using namespace Supernova;

Sprite::Sprite(): Image(){
}

Sprite::~Sprite(){
    if (defaultAnimation)
        delete defaultAnimation;
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

unsigned int Sprite::getFramesSize(){
    return (unsigned int)framesRect.size();
}

bool Sprite::isAnimation(){
    if (defaultAnimation)
        return defaultAnimation->isRunning();

    return false;
}

void Sprite::playAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(framesTime, frames, loop);
    addAction(defaultAnimation);
    defaultAnimation->play();
}

void Sprite::playAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(framesTime, startFrame, endFrame, loop);
    addAction(defaultAnimation);
    defaultAnimation->play();
}

void Sprite::playAnimation(int interval, int startFrame, int endFrame, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(interval, startFrame, endFrame, loop);
    addAction(defaultAnimation);
    defaultAnimation->play();
}

void Sprite::stopAnimation(){
    if (defaultAnimation)
        defaultAnimation->stop();
}

bool Sprite::draw(){
    
    return Image::draw();
}
