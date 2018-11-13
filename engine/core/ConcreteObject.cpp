#include "ConcreteObject.h"
#include "Scene.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

ConcreteObject::ConcreteObject(): Object(){
    visible = true;
    transparent = false;
    distanceToCamera = -1;

    minBufferSize = 0;

    render = NULL;
    shadowRender = NULL;

    body = NULL;
}

ConcreteObject::~ConcreteObject(){

}

void ConcreteObject::updateBuffer(int index){
    if (index == 0) {
        render->setVertexSize(buffers[index].getCount());
        if (shadowRender)
            shadowRender->setVertexSize(buffers[index].getCount());
    }
    render->updateVertexBuffer(buffers[index].getName(), buffers[index].getSize() * sizeof(float), buffers[index].getBuffer());
    if (shadowRender)
        shadowRender->updateVertexBuffer(buffers[index].getName(), buffers[index].getSize() * sizeof(float), buffers[index].getBuffer());
}

Matrix4 ConcreteObject::getNormalMatrix(){
    return normalMatrix;
}

unsigned int ConcreteObject::getMinBufferSize(){
    return minBufferSize;
}

void ConcreteObject::setColor(Vector4 color){
    if (color.w != 1){
        transparent = true;
    }
    material.setColor(color);
}

void ConcreteObject::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 ConcreteObject::getColor(){
    return *material.getColor();
}

void ConcreteObject::setVisible(bool visible){
    this->visible = visible;
}

bool ConcreteObject::isVisible(){
    return visible;
}

void ConcreteObject::setTexture(Texture* texture){
    
    Texture* oldTexture = material.getTexture();
    
    if (texture != oldTexture){
        
        material.setTexture(texture);
        
        if (loaded){
            textureLoad();
        }
        
    }
}

void ConcreteObject::setTexture(std::string texturepath){
    
    std::string oldTexture = material.getTexturePath();
    
    if (texturepath != oldTexture){
        
        material.setTexturePath(texturepath);
        
        if (loaded){
            textureLoad();
        }
        
    }
}

Material* ConcreteObject::getMaterial(){
    return &this->material;
}

std::string ConcreteObject::getTexture(){
    return material.getTexturePath();
}

void ConcreteObject::updateDistanceToCamera(){
    distanceToCamera = (this->cameraPosition - this->getWorldPosition()).length();
}

void ConcreteObject::setSceneTransparency(bool transparency){
    if (scene) {
        if (scene->getUserDefinedTransparency() != S_OPTION_NO)
            scene->useTransparency = transparency;
    }
}

void ConcreteObject::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Object::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updateDistanceToCamera();
}

void ConcreteObject::updateMatrix(){
    Object::updateMatrix();
    
    this->normalMatrix.identity();

    updateDistanceToCamera();
}

bool ConcreteObject::draw(){

    if (scene && scene->isDrawingShadow()){
        shadowDraw();
    }else{
        if (transparent && scene && scene->useDepth && distanceToCamera >= 0){
            scene->transparentQueue.insert(std::make_pair(distanceToCamera, this));
        }else{
            renderDraw();
        }

        if (transparent){
            setSceneTransparency(true);
        }
    }

    return Object::draw();
}

bool ConcreteObject::load(){
    Object::load();

    if (material.isTransparent()){
        transparent = true;
        setSceneTransparency(true);
    }
    
    shadowLoad();

    return true;
}

bool ConcreteObject::textureLoad(){
    
    return true;
}

bool ConcreteObject::shadowLoad(){
    
    return true;
}

bool ConcreteObject::shadowDraw(){

    return true;
}

bool ConcreteObject::renderDraw(){

    return true;
}
