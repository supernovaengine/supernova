#include "ConcreteObject.h"
#include "Scene.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

ConcreteObject::ConcreteObject(): Object(){
    transparent = false;
    distanceToCamera = -1;

    minBufferSize = 0;

    body = NULL;
}

ConcreteObject::~ConcreteObject(){

}

Matrix4 ConcreteObject::getNormalMatrix(){
    return normalMatrix;
}

unsigned int ConcreteObject::getMinBufferSize(){
    return minBufferSize;
}

void ConcreteObject::setPosition(Vector3 position){
    if (body)
        body->setPosition(position);
    
    Object::setPosition(position);
}

void ConcreteObject::setRotation(Quaternion rotation){
    if (body)
        body->setRotation(rotation);
    
    Object::setRotation(rotation);
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

void ConcreteObject::attachBody(Body* body){
    if (!body->attachedObject){
        this->body = body;
        body->attachedObject = this;
    
        body->setPosition(position);
        body->setRotation(rotation);
    }else{
        Log::Error("Body is attached with other object already");
    }
}

void ConcreteObject::detachBody(){
    this->body->attachedObject = NULL;
    this->body = NULL;
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

    if (body){
        bool needUpdate = false;
        Vector3 bodyPosition = body->getPosition();
        Quaternion bodyRotation = body->getRotation();
        
        if (getPosition() != bodyPosition){
            position = bodyPosition;
            needUpdate = true;
        }
        
        if (getRotation() != bodyRotation){
            rotation = bodyRotation;
            needUpdate = true;
        }
        
        if (needUpdate)
            updateMatrix();
    }

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
