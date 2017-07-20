#include "ObjectRender.h"

#include "Engine.h"
#include "gles2/GLES2Object.h"

using namespace Supernova;


ObjectRender::ObjectRender(){
    minBufferSize = 0;
    indexAttribute.data = NULL;
    
    lighting = false;
    hasfog = false;
    
    sceneRender = NULL;
    texture = NULL;
    program = NULL;
}

ObjectRender* ObjectRender::newInstance(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Object();
    }
    
    return NULL;
}

ObjectRender::~ObjectRender(){

}

void ObjectRender::setTexture(Texture* texture){
    this->texture = texture;
}

void ObjectRender::setMinBufferSize(unsigned int minBufferSize){
    this->minBufferSize = minBufferSize;
}

void ObjectRender::addVertexAttribute(int type, unsigned int elements, unsigned long size, void* data){
    vertexAttributes[type] = { elements, size, data };
}

void ObjectRender::addIndex(unsigned long size, void* data){
    indexAttribute = { 1, size, data };
}

void ObjectRender::addProperty(int type, int datatype, unsigned long size, void* data){
    properties[type] = { datatype, size, data };
}

void ObjectRender::checkLighting(){
    lighting = false;
    if (sceneRender != NULL){
        lighting = sceneRender->lighting;
    }
}

void ObjectRender::checkFog(){
    hasfog = false;
    if ((sceneRender != NULL) && (sceneRender->getFog() != NULL)){
        hasfog = true;
    }
}

bool ObjectRender::load(){
    checkLighting();
    checkFog();
    
    if (!program)
        program = new Program();
    
    if (texture)
        texture->load();
    
    return true;
}

bool ObjectRender::draw(){
    return true;
}

void ObjectRender::destroy(){
    if (texture)
        texture->destroy();
}
