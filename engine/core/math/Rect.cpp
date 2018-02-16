#include "Rect.h"


using namespace Supernova;

Rect::Rect(){
    x = 0;
    y = 0;
    width = 1;
    height = 1;
}

Rect::Rect(float x, float y, float width, float height){
    setRect(x, y, width, height);
}

Rect::Rect(const Rect& t){
    this->x = t.x;
    this->y = t.y;
    this->width = t.width;
    this->height = t.height;
}

Rect& Rect::operator = (const Rect& t){
    this->x = t.x;
    this->y = t.y;
    this->width = t.width;
    this->height = t.height;
    
    return *this;
}

bool Rect::operator == (const Rect& t){
    return ((this->x == t.x) && (this->y == t.y) && (this->width == t.width) && (this->height == t.height));
}

bool Rect::operator != (const Rect& t){
    return ((this->x != t.x) || (this->y != t.y) || (this->width != t.width) || (this->height != t.height));
}

float Rect::getX(){
    return x;
}

float Rect::getY(){
    return y;
}

float Rect::getWidth(){
    return width;
}

float Rect::getHeight(){
    return height;
}

float* Rect::ptr(){
    return &x;
}

void Rect::setRect(float x, float y, float width, float height){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void Rect::setRect(Rect rect){
    setRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

bool Rect::isNormalized(){
    if ((x >= 0 && x <= 1) && (y >= 0 && y <= 1) && (width >= 0 && width <= 1) && (height >= 0 && height <= 1)){
        return true;
    }
    return false;
}
