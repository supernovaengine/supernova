
#include "Mesh2D.h"
#include "Engine.h"
#include "Scene.h"

Mesh2D::Mesh2D(): Mesh(){
    this->width = 0;
    this->height = 0;

    this->billboard = false;
    this->fixedSizeBillboard=false;
    this->billboardScaleFactor=100;

    this->clipping = false;
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

bool Mesh2D::draw(){
    if (clipping) {

        float scaleX = scale.x;
        float scaleY = scale.y;
        Object *parent = this->parent;
        while (parent) {
            scaleX *= parent->getScale().x;
            scaleY *= parent->getScale().y;
            parent = parent->getParent();
        }

        int objScreenPosX = (int)(getWorldPosition().x * ((float) Engine::getScreenWidth() /
                                                    (float) Engine::getCanvasWidth()));
        int objScreenPosY = (int)(getWorldPosition().y * ((float) Engine::getScreenHeight() /
                                                    (float) Engine::getCanvasHeight()));
        int objScreenWidth = (int)(width * scaleX * ((float) Engine::getScreenWidth() /
                                               (float) Engine::getCanvasWidth()));
        int objScreenHeight = (int)(height * scaleY * ((float) Engine::getScreenHeight() /
                                                 (float) Engine::getCanvasHeight()));

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

    }else{
        return Mesh::draw();
    }
}
