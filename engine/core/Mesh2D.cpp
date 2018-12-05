
#include "Mesh2D.h"
#include "Engine.h"
#include "Scene.h"
#include "Log.h"

using namespace Supernova;

Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;

    this->billboard = false;
    this->fixedSizeBillboard=false;
    this->billboardScaleFactor=100;

    this->clipping = false;
    this->clipBorder[0] = 0;
    this->clipBorder[1] = 0;
    this->clipBorder[2] = 0;
    this->clipBorder[3] = 0;
    this->invertTexture = false;
}

Mesh2D::~Mesh2D(){

}

float Mesh2D::convTex(float value){
    if (invertTexture)
        return 1.0 - value;

    return value;
}

void Mesh2D::updateMatrix(){

    if (billboard) {
        if (viewMatrix) {

            rotation.fromRotationMatrix(viewMatrix->transpose());
            if (parent)
                rotation = parent->getWorldRotation().inverse() * rotation;

            if (fixedSizeBillboard){
                float scalebillboard = (modelViewProjectionMatrix * Vector4(0, 0, 0, 1.0)).w / billboardScaleFactor;
                scale = Vector3(scalebillboard, scalebillboard, scalebillboard);
            }
        }
    }

    Mesh::updateMatrix();
}

void Mesh2D::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){

    Mesh::updateVPMatrix( viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    if (billboard) {
        updateMatrix();
    }
}

void Mesh2D::setSize(int width, int height){
    this->width = width;
    this->height = height;
}

void Mesh2D::setInvertTexture(bool invertTexture){
    this->invertTexture = invertTexture;
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

void Mesh2D::setClipping(bool clipping){
    this->clipping = clipping;
}

void Mesh2D::setClipBorder(float left, float top, float right, float bottom){
    this->clipBorder[0] = left;
    this->clipBorder[1] = top;
    this->clipBorder[2] = right;
    this->clipBorder[3] = bottom;
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

bool Mesh2D::draw() {

    if (clipping && scene && scene->getCamera()->getType() == S_CAMERA_2D && scene->getTextureRender() == NULL) {

        float scaleX = getWorldScale().x;
        float scaleY = getWorldScale().y;

        float tempX = (2 * getWorldPosition().x / (float) Engine::getCanvasWidth()) - 1;
        float tempY = (2 * getWorldPosition().y / (float) Engine::getCanvasHeight()) - 1;

        float widthRatio = scaleX * (Engine::getViewRect()->getWidth() / (float) Engine::getCanvasWidth());
        float heightRatio = scaleY * (Engine::getViewRect()->getHeight() / (float) Engine::getCanvasHeight());

        int objScreenPosX = (tempX * Engine::getViewRect()->getWidth() + (float) Engine::getScreenWidth()) / 2;
        int objScreenPosY = (tempY * Engine::getViewRect()->getHeight() + (float) Engine::getScreenHeight()) / 2;
        int objScreenWidth = width * widthRatio;
        int objScreenHeight = height * heightRatio;

        if (scene && scene->getScene()->is3D())
            objScreenPosY = (float) Engine::getScreenHeight() - objScreenHeight - objScreenPosY;


        if (!(clipBorder[0] == 0 && clipBorder[1] == 0 && clipBorder[2] == 0 && clipBorder[3] == 0)){
            float borderScreenLeft = clipBorder[0] * widthRatio;
            float borderScreenTop = clipBorder[1] * heightRatio;
            float borderScreenRight = clipBorder[2] * widthRatio;
            float borderScreenBottom = clipBorder[3] * heightRatio;

            objScreenPosX += borderScreenLeft;
            objScreenPosY += borderScreenTop;
            objScreenWidth -= (borderScreenLeft + borderScreenRight);
            objScreenHeight -= (borderScreenTop + borderScreenBottom);
        }

        scissor.setRect(objScreenPosX, objScreenPosY, objScreenWidth, objScreenHeight);

    }else if (clipping){
        clipping = false;
        scissor.setRect(0, 0, 0, 0);
        Log::Error("Can not clipping object with 2D camera or textureRender scene");
    }

    return Mesh::draw();
}
