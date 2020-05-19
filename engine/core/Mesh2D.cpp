
#include "Mesh2D.h"
#include "Engine.h"
#include "system/System.h"
#include "Scene.h"
#include "Log.h"
#include "math/Angle.h"

using namespace Supernova;

Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;

    this->billboard = false;
    this->fakeBillboard = true;
    this->fixedSizeBillboard=false;
    this->cylindricalBillboard=false;
    this->billboardScaleFactor=1000;

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

void Mesh2D::updateMVPMatrix(){

    if (billboard && fakeBillboard) {

        Matrix4 modelViewMatrix = *viewMatrix * modelMatrix;

        Vector3 newScale = getWorldScale();

        if (fixedSizeBillboard) {
            float scalebillboard =
                    (((*this->viewProjectionMatrix)) * Vector4(worldPosition, 1.0)).w / billboardScaleFactor;
            newScale = newScale * scalebillboard;
        }

        modelViewMatrix.set(0,0, newScale.x);
        modelViewMatrix.set(0,1, 0.0);
        modelViewMatrix.set(0,2, 0.0);

        if (!cylindricalBillboard) {
            modelViewMatrix.set(1, 0, 0.0);
            modelViewMatrix.set(1, 1, newScale.y);
            modelViewMatrix.set(1, 2, 0.0);
        }

        modelViewMatrix.set(2,0, 0.0);
        modelViewMatrix.set(2,1, 0.0);
        modelViewMatrix.set(2,2, newScale.z);


        if (this->viewProjectionMatrix != NULL){
            this->modelViewProjectionMatrix = *projectionMatrix * modelViewMatrix;
        }

    }else{
        Mesh::updateMVPMatrix();
    }
}

void Mesh2D::updateModelMatrix(){

    Mesh::updateModelMatrix();

    if (billboard && !fakeBillboard && viewProjectionMatrix) {

        if (scene && scene->getCamera()) {
            Vector3 camPos = scene->getCamera()->getWorldPosition();

            if (cylindricalBillboard)
                camPos.y = worldPosition.y;

            lookAt(camPos, *scene->getCamera()->getWorldUpPtr());

            if (fixedSizeBillboard) {
                float scalebillboard =
                        (((*this->viewProjectionMatrix)) * Vector4(worldPosition, 1.0)).w / billboardScaleFactor;

                if (billboardOldScale == Vector3::ZERO)
                    billboardOldScale = scale;

                setScale(billboardOldScale * scalebillboard);
            }
        }

    }
}

void Mesh2D::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){

    Mesh::updateVPMatrix( viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    if (billboard && !fakeBillboard) {
        needUpdate();
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

void Mesh2D::setFakeBillboard(bool fakeBillboard){
    this->fakeBillboard = fakeBillboard;
}

void Mesh2D::setFixedSizeBillboard(bool fixedSizeBillboard){
    this->fixedSizeBillboard = fixedSizeBillboard;
}

void Mesh2D::setBillboardScaleFactor(float billboardScaleFactor){
    this->billboardScaleFactor = billboardScaleFactor;
}

void Mesh2D::setCylindricalBillboard(bool cylindricalBillboard) {
    this->cylindricalBillboard = cylindricalBillboard;
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

    if (clipping && scene && scene->getCamera()->getType() == S_CAMERA_2D) {

        int objScreenPosX = 0;
        int objScreenPosY = 0;
        int objScreenWidth = 0;
        int objScreenHeight = 0;

        if (!scene->getTextureFrame()) {

            float scaleX = getWorldScale().x;
            float scaleY = getWorldScale().y;

            float tempX = (2 * getWorldPosition().x / (float) Engine::getCanvasWidth()) - 1;
            float tempY = (2 * getWorldPosition().y / (float) Engine::getCanvasHeight()) - 1;

            float widthRatio = scaleX * (Engine::getViewRect()->getWidth() / (float) Engine::getCanvasWidth());
            float heightRatio = scaleY * (Engine::getViewRect()->getHeight() / (float) Engine::getCanvasHeight());

            objScreenPosX = (tempX * Engine::getViewRect()->getWidth() + (float) System::instance()->getScreenWidth()) / 2;
            objScreenPosY = (tempY * Engine::getViewRect()->getHeight() + (float) System::instance()->getScreenHeight()) / 2;
            objScreenWidth = width * widthRatio;
            objScreenHeight = height * heightRatio;

            if (!scene->getScene()->is3D())
                objScreenPosY = (float) System::instance()->getScreenHeight() - objScreenHeight - objScreenPosY;


            if (!(clipBorder[0] == 0 && clipBorder[1] == 0 && clipBorder[2] == 0 && clipBorder[3] == 0)) {
                float borderScreenLeft = clipBorder[0] * widthRatio;
                float borderScreenTop = clipBorder[1] * heightRatio;
                float borderScreenRight = clipBorder[2] * widthRatio;
                float borderScreenBottom = clipBorder[3] * heightRatio;

                objScreenPosX += borderScreenLeft;
                objScreenPosY += borderScreenTop;
                objScreenWidth -= (borderScreenLeft + borderScreenRight);
                objScreenHeight -= (borderScreenTop + borderScreenBottom);
            }

        }else {

            objScreenPosX = getWorldPosition().x;
            objScreenPosY = getWorldPosition().y;
            objScreenWidth = width;
            objScreenHeight = height;

            if (!scene->getScene()->is3D())
                objScreenPosY = (float) scene->getTextureFrame()->getTextureFrameHeight() - objScreenHeight - objScreenPosY;

            if (!(clipBorder[0] == 0 && clipBorder[1] == 0 && clipBorder[2] == 0 && clipBorder[3] == 0)) {
                float borderScreenLeft = clipBorder[0];
                float borderScreenTop = clipBorder[1];
                float borderScreenRight = clipBorder[2];
                float borderScreenBottom = clipBorder[3];

                objScreenPosX += borderScreenLeft;
                objScreenPosY += borderScreenTop;
                objScreenWidth -= (borderScreenLeft + borderScreenRight);
                objScreenHeight -= (borderScreenTop + borderScreenBottom);
            }

        }

        scissor.setRect(objScreenPosX, objScreenPosY, objScreenWidth, objScreenHeight);

    }else if (clipping){
        clipping = false;
        scissor.setRect(0, 0, 0, 0);
        Log::Error("Can not clipping object with 2D camera or textureRender scene");
    }

    return Mesh::draw();
}