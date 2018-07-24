#include "Sprite.h"
#include "Log.h"
#include "Engine.h"

using namespace Supernova;

Sprite::Sprite(): Image(){
}

Sprite::~Sprite(){
    if (defaultAnimation)
        delete defaultAnimation;
}

std::vector<int> Sprite::findFramesByString(std::string name){
    std::vector<int> frameslist;

    std::map<int,frameData>::iterator it = framesRect.begin();
    for (std::pair<int,frameData> frameRect : framesRect) {
        std::size_t found = frameRect.second.name.find(name);
        if (found!=std::string::npos)
            frameslist.push_back(frameRect.first);
    }

    return frameslist;
}

void Sprite::addFrame(int id, std::string name, Rect rect){
    framesRect[id] = {name, rect};
}

void Sprite::addFrame(std::string name, float x, float y, float width, float height){
    int id = 0;
    while ( framesRect.count(id) > 0 ) {
        id++;
    }
    addFrame(id, name, Rect(x, y, width, height));
}

void Sprite::addFrame(float x, float y, float width, float height){
    addFrame("", x, y, width, height);
}

void Sprite::addFrame(Rect rect){
    addFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Sprite::removeFrame(int id){
    framesRect.erase(id);
}

void Sprite::removeFrame(std::string name){
    std::vector<int> frameslist = findFramesByString(name);

    while (frameslist.size() > 0) {
        framesRect.erase(frameslist[0]);
        frameslist.clear();
        frameslist = findFramesByString(name);
    }
}

void Sprite::setFrame(int id){
    if (framesRect.count(id) > 0) {
        setTextureRect(framesRect[id].rect);
    }else{
        setTextureRect(Rect(0, 0, 1, 1));
    }
}

void Sprite::setFrame(std::string name){
    std::vector<int> frameslist = findFramesByString(name);
    if (frameslist.size() > 0){
        setFrame(frameslist[0]);
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

void Sprite::runAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(framesTime, frames, loop);
    addAction(defaultAnimation);
    defaultAnimation->run();
}

void Sprite::runAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(framesTime, startFrame, endFrame, loop);
    addAction(defaultAnimation);
    defaultAnimation->run();
}

void Sprite::runAnimation(int interval, int startFrame, int endFrame, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(interval, startFrame, endFrame, loop);
    addAction(defaultAnimation);
    defaultAnimation->run();
}

void Sprite::runAnimation(int interval, std::vector<int> frames, bool loop){
    if (defaultAnimation) {
        delete defaultAnimation;
    }

    defaultAnimation = new SpriteAnimation(interval, frames, loop);
    addAction(defaultAnimation);
    defaultAnimation->run();
}

void Sprite::stopAnimation(){
    if (defaultAnimation)
        defaultAnimation->stop();
}

bool Sprite::load(){

    if (meshnodes[0]->getMaterial()->getTexture()) {
        meshnodes[0]->getMaterial()->getTexture()->setResampleToPowerOfTwo(false);
    }

    return Image::load();
}

bool Sprite::draw(){
    
    return Image::draw();
}
