
#include "Mesh2D.h"


Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::setSize(int width, int height){

    if ((this->width != width || this->height != height) && this->width >= 0 && this->height >= 0){
        this->width = width;
        this->height = height;
        if (loaded)
            load();
    }
}

void Mesh2D::setWidth(int width){
    setSize(width, this->height);
}

int Mesh2D::getWidth(){
    return width;
}

void Mesh2D::setHeight(int height){
    setSize(this->width, height);
}

int Mesh2D::getHeight(){
    return height;
}
