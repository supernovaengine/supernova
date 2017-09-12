#include "ConcreteObject.h"
#include "Scene.h"

using namespace Supernova;

ConcreteObject::ConcreteObject(): Object(){
    transparent = false;
    distanceToCamera = -1;

    minBufferSize = 0;
}

ConcreteObject::~ConcreteObject(){

}

Matrix4 ConcreteObject::getNormalMatrix(){
    return normalMatrix;
}

unsigned int ConcreteObject::getMinBufferSize(){
    return minBufferSize;
}

void ConcreteObject::setColor(float red, float green, float blue, float alpha){
    if (alpha != 1){
        transparent = true;
    }else{
        transparent = false;
    }
    material.setColor(Vector4(red, green, blue, alpha));
}

void ConcreteObject::setTexture(Texture* texture){
    
    Texture* oldTexture = material.getTexture();
    
    if (texture != oldTexture){
        
        material.setTexture(texture);
        
        if (loaded){
            //TODO: Not working with reload() because destroy delete new texture
            load();
        }
        
    }
}

void ConcreteObject::setTexture(std::string texturepath){
    
    std::string oldTexture = material.getTexturePath();
    
    if (texturepath != oldTexture){
        
        material.setTexturePath(texturepath);
        
        if (loaded){
            //TODO: Not working with reload() because destroy delete new texture
            load();
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
    if (this->cameraPosition != NULL){
        distanceToCamera = ((*this->cameraPosition) - this->getWorldPosition()).length();
    }
}

void ConcreteObject::setTransparency(bool transparency){
    if (scene != NULL && transparent == true) {
        ((Scene*)scene)->useTransparency = true;
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

    if (scene && scene->shadowMode){
        shadowDraw();
    }else{
        if (transparent && scene && scene->useDepth && distanceToCamera >= 0){
            scene->transparentQueue.insert(std::make_pair(distanceToCamera, this));
        }else{
            renderDraw();
        }

        if (transparent){
            setTransparency(true);
        }
    }

    return Object::draw();
}

bool ConcreteObject::load(){
    Object::load();

    if (material.getTexture()) {
        if (material.getTexture()->getType() == S_TEXTURE_2D)
            transparent = material.getTexture()->hasAlphaChannel();
    }
    if (transparent){
        setTransparency(true);
    }
    
    shadowLoad();

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
