#include "Sprite.h"

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
