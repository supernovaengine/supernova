#include "ObjectRender.h"

#include "Engine.h"
#include "gles2/GLES2Object.h"

using namespace Supernova;


ObjectRender::ObjectRender(){
    minBufferSize = 0;
    primitiveType = 0;
    indexAttribute.data = NULL;
    programOwned = false;
    programShader = -1;
    dynamicBuffer = false;
    
    hasLight = false;
    hasFog = false;
    hasTextureRect = false;
    
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
    if (program && programOwned)
        delete program;
}

void ObjectRender::setTexture(Texture* texture){
    this->texture = texture;
}

void ObjectRender::setProgram(Program* program){
    if (this->program && programOwned)
        delete this->program;
    
    this->program = program;
    programOwned = false;
}

void ObjectRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void ObjectRender::setMinBufferSize(unsigned int minBufferSize){
    this->minBufferSize = minBufferSize;
}

void ObjectRender::setPrimitiveType(int primitiveType){
    this->primitiveType = primitiveType;
}

void ObjectRender::setProgramShader(int programShader){
    this->programShader = programShader;
}

void ObjectRender::addVertexAttribute(int type, unsigned int elements, unsigned long size, void* data){
    if (data && (size > 0))
        vertexAttributes[type] = { elements, size, data };
}

void ObjectRender::setDynamicBuffer(bool dynamicBuffer){
    this->dynamicBuffer = dynamicBuffer;
}

void ObjectRender::addIndex(unsigned long size, void* data){
    if (data && (size > 0))
        indexAttribute = { 1, size, data };
}

void ObjectRender::addProperty(int type, int datatype, unsigned long size, void* data){
    if (data && (size > 0))
        properties[type] = { datatype, size, data };
}

void ObjectRender::updateVertexAttribute(int type, unsigned long size){
    if (size > 0)
        vertexAttributes[type].size = size;
}

void ObjectRender::checkLighting(){
    hasLight = false;
    if (sceneRender != NULL){
        hasLight = sceneRender->lighting;
    }
}

void ObjectRender::checkFog(){
    hasFog = false;
    if ((sceneRender != NULL) && (sceneRender->getFog() != NULL)){
        hasFog = true;
    }
}

void ObjectRender::checkTextureRect(){
    hasTextureRect = false;
    if (vertexAttributes.count(S_VERTEXATTRIBUTE_TEXTURERECTS)){
        hasTextureRect = true;
    }
}

bool ObjectRender::load(){
    checkLighting();
    checkFog();
    checkTextureRect();
    
    if (!program){
        program = new Program();
        programOwned = true;
    }
    
    if (programShader != -1)
        program->setShader(programShader);
    
    program->setDefinitions(hasLight, hasFog, hasTextureRect);
    program->load();
    
    if (texture)
        texture->load();
    
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end();)
    {
        if (!program->existVertexAttribute(it->first)){
            it = vertexAttributes.erase(it);
        }else{
            it++;
        }
    }
    
    for (std::unordered_map<int, propertyData>::iterator it = properties.begin(); it != properties.end();)
    {
        if (!program->existProperty(it->first)){
            it = properties.erase(it);
        }else{
            it++;
        }
    }
    
    return true;
}

bool ObjectRender::draw(){
    return true;
}

void ObjectRender::destroy(){
    if (texture)
        texture->destroy();
}
