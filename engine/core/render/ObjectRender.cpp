#include "ObjectRender.h"

#include "Engine.h"
#include "gles2/GLES2Object.h"

using namespace Supernova;


ObjectRender::ObjectRender(){
    minBufferSize = 0;
    primitiveType = 0;
    vertexAttributes.clear();
    indexAttribute.data = NULL;
    properties.clear();
    programOwned = false;
    programShader = -1;
    dynamicBuffer = false;
    
    hasLight = false;
    hasFog = false;
    hasTextureCoords = false;
    hasTextureRect = false;
    hasTextureCube = false;
    isSky = false;
    isText = false;
    
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

void ObjectRender::addIndex(unsigned long size, void* data){    
    if (data && (size > 0))
        indexAttribute = { size, data };
}

void ObjectRender::addProperty(int type, int datatype, unsigned long size, void* data){
    if (data && (size > 0))
        properties[type] = { datatype, size, data };
}

void ObjectRender::updateVertexAttribute(int type, unsigned long size, void* data){
    if (vertexAttributes.count(type))
        addVertexAttribute(type, vertexAttributes[type].elements, size, data);
}

void ObjectRender::updateIndex(unsigned long size, void* data){
    if (indexAttribute.data)
        addIndex(size, data);
}

void ObjectRender::setDynamicBuffer(bool dynamicBuffer){
    this->dynamicBuffer = dynamicBuffer;
}

void ObjectRender::setHasTextureCoords(bool hasTextureCoords){
    this->hasTextureCoords = hasTextureCoords;
}

void ObjectRender::setHasTextureRect(bool hasTextureRect){
    this->hasTextureRect = hasTextureRect;
}

void ObjectRender::setHasTextureCube(bool hasTextureCube){
    this->hasTextureCube = hasTextureCube;
}

void ObjectRender::setIsSky(bool isSky){
    this->isSky = isSky;
}

void ObjectRender::setIsText(bool isText){
    this->isText = isText;
}

Program* ObjectRender::getProgram(){
    
    loadProgram();
    
    return program;
}

void ObjectRender::checkLighting(){
    if (sceneRender != NULL){
        hasLight = sceneRender->lighting;
    }
}

void ObjectRender::checkFog(){
    if ((sceneRender != NULL) && (sceneRender->getFog() != NULL)){
        hasFog = true;
    }
}

void ObjectRender::checkTextureCoords(){
    if (texture){
        hasTextureCoords = true;
    }
}

void ObjectRender::checkTextureRect(){
    if (vertexAttributes.count(S_VERTEXATTRIBUTE_TEXTURERECTS)){
        hasTextureRect = true;
    }
    if (properties.count(S_PROPERTY_TEXTURERECT)){
        hasTextureRect = true;
    }
}

void ObjectRender::checkTextureCube(){
    if (texture && texture->getType() == S_TEXTURE_CUBE){
        hasTextureCube = true;
    }
}

void ObjectRender::loadProgram(){
    checkLighting();
    checkFog();
    checkTextureCoords();
    checkTextureRect();
    checkTextureCube();
    
    if (!program){
        program = new Program();
        programOwned = true;
    }
    
    if (programOwned){
        if (programShader != -1)
            program->setShader(programShader);
    
        program->setDefinitions(hasLight, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText);
        program->load();
    }
}

bool ObjectRender::load(){
    
    loadProgram();
    
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

bool ObjectRender::prepareDraw(){
    return true;
}

bool ObjectRender::draw(){
    return true;
}

bool ObjectRender::finishDraw(){
    return true;
}

void ObjectRender::destroy(){
    if (texture)
        texture->destroy();
    
    if (program)
        program->destroy();
}
