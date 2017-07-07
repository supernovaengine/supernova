#include "Sprite.h"

using namespace Supernova;

Sprite::Sprite(): RectImage(){

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
    setRect(framesRect[id]);
}

void Sprite::setFrame(int id){
    setFrame(std::to_string(id));
}
