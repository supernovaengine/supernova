
#include "Mesh2D.h"
#include "Engine.h"
#include "Scene.h"
#include "platform/Log.h"

using namespace Supernova;

Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;

    this->billboard = false;
    this->fixedSizeBillboard=false;
    this->billboardScaleFactor=100;

    this->clipping = false;
    this->invertTexture = false;
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::updateMatrix(){

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

        float scaleX = scale.x;
        float scaleY = scale.y;
        Object *parent = this->parent;
        while (parent) {
            scaleX *= parent->getScale().x;
            scaleY *= parent->getScale().y;
            parent = parent->getParent();
        }

        float tempX = (2 * getWorldPosition().x / (float) Engine::getCanvasWidth()) - 1;
        float tempY = (2 * getWorldPosition().y / (float) Engine::getCanvasHeight()) - 1;

        int objScreenPosX = (tempX * Engine::getViewRect()->getWidth() + (float) Engine::getScreenWidth()) / 2;
        int objScreenPosY = (tempY * Engine::getViewRect()->getHeight() + (float) Engine::getScreenHeight()) / 2;
        int objScreenWidth = width * scaleX * (Engine::getViewRect()->getWidth() / (float) Engine::getCanvasWidth());
        int objScreenHeight = height * scaleY * (Engine::getViewRect()->getHeight() / (float) Engine::getCanvasHeight());

        if (scene && scene->getScene()->is3D())
            objScreenPosY = (float) Engine::getScreenHeight() - objScreenHeight - objScreenPosY;

        SceneRender* sceneRender = scene->getSceneRender();

        bool on = sceneRender->isEnabledScissor();

        Rect rect = sceneRender->getActiveScissor();

        if (on) {
            if (objScreenPosX < rect.getX())
                objScreenPosX = rect.getX();

            if (objScreenPosY < rect.getY())
                objScreenPosY = rect.getY();

            if (objScreenPosX + objScreenWidth >= rect.getX() + rect.getWidth())
                objScreenWidth = rect.getX() + rect.getWidth() - objScreenPosX;

            if (objScreenPosY + objScreenHeight >= rect.getY() + rect.getHeight())
                objScreenHeight = rect.getY() + rect.getHeight() - objScreenPosY;
        }

        sceneRender->enableScissor(Rect(objScreenPosX, objScreenPosY, objScreenWidth, objScreenHeight));

        bool drawReturn = Mesh::draw();

        if (!on)
            sceneRender->disableScissor();

        return drawReturn;

    }else if (clipping){
        clipping = false;
        Log::Error(LOG_TAG, "Can not clipping object with 2D camera or textureRender scene");
        return Mesh::draw();
    }else{
        return Mesh::draw();
    }
}
