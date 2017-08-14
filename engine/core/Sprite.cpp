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

std::vector<int> Sprite::findFramesByString(std::string id){
    std::vector<int> frameslist;
    for (int i = 0; i < framesRect.size(); i++){

        std::size_t found = framesRect[i].id.find(id);
        if (found!=std::string::npos)
            frameslist.push_back(i);
    }
    return frameslist;
}

void Sprite::addFrame(float x, float y, float width, float height){
    framesRect.push_back({"", Rect(x, y, width, height)});
}

void Sprite::addFrame(std::string id, float x, float y, float width, float height){
    framesRect.push_back({id, Rect(x, y, width, height)});
}

void Sprite::removeFrame(int index){
    framesRect.erase(framesRect.begin() + index);
}

void Sprite::removeFrame(std::string id){
    std::vector<int> frameslist = findFramesByString(id);

    while (frameslist.size() > 0) {
        framesRect.erase(framesRect.begin() + frameslist[0]);
        frameslist.clear();
        frameslist = findFramesByString(id);
    }
}

void Sprite::setFrame(std::string id){
    std::vector<int> frameslist = findFramesByString(id);
    if (frameslist.size() > 0){
        setTectureRect(framesRect[frameslist[0]].rect);
    }
}

void Sprite::setFrame(int id){
    if (id >= 0 && id < framesRect.size()){
        setTectureRect(framesRect[id].rect);
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

void Sprite::playAnimation(int interval, std::vector<int> frames, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(interval, frames, loop);
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
