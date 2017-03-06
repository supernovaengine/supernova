
#include "Mesh2D.h"


Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;
    this->billboard = false;
    this->fixedSizeBillboard=false;
    this->billboardScaleFactor=100;
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::update(){

    if (billboard) {
        if (viewMatrix) {

            rotation.fromRotationMatrix(viewMatrix->getTranspose());
            if (parent)
                rotation = parent->getWorldRotation().inverse() * rotation;

            if (fixedSizeBillboard){
                float scalebillboard = (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w / billboardScaleFactor;
                scale = Vector3(scalebillboard, scalebillboard, scalebillboard);
            }
        }
    }

    Mesh::update();
}

void Mesh2D::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){

    Mesh::transform( viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    if (billboard) {
        update();
    }
}

void Mesh2D::setSize(int width, int height){

    if ((this->width != width || this->height != height) && this->width >= 0 && this->height >= 0){
        this->width = width;
        this->height = height;
        if (loaded)
            load();
    }
}

void Mesh2D::setBillboard(bool billboard){
    this->billboard = billboard;
}

void Mesh2D::setFixedSizeBillboard(bool fixedSizeBillboard){
    this->fixedSizeBillboard = fixedSizeBillboard;
}

void Mesh2D::setBillboardScaleFactor(float billboardScaleFactor){
    this->billboardScaleFactor = billboardScaleFactor;
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
