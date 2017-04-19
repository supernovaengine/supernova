#include "Sprite.h"
#include "render/TextureManager.h"


Sprite::Sprite(): RectImage(){

}

Sprite::~Sprite(){

}

void Sprite::addFrame(std::string id, float x, float y, float width, float height){
    framesRect[id] = TextureRect(x, y, width, height);
}

void Sprite::removeFrame(std::string id){
    framesRect.erase(id);
}

void Sprite::setFrame(std::string id){
    setRect(framesRect[id]);
}

void Sprite::setFrame(int id){
    setRect(framesRect[std::to_string(id)]);
}
