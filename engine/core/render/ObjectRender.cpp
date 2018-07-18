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

    vertexSize = 0;
    numLights = 0;
    numShadows2D = 0;
    numShadowsCube = 0;
    hasFog = false;
    hasTextureCoords = false;
    hasTextureRect = false;
    hasTextureCube = false;
    isSky = false;
    isText = false;
    
    sceneRender = NULL;
    lightRender = NULL;
    fogRender = NULL;
    
    texture = NULL;
    program = NULL;
    shadowsMap2D.clear();
    shadowsMapCube.clear();
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

    if (lightRender)
        delete lightRender;

    if (fogRender)
        delete fogRender;
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

void ObjectRender::setShadowsMap2D(std::vector<Texture*> shadowsMap2D){
    this->shadowsMap2D = shadowsMap2D;
}

void ObjectRender::setShadowsMapCube(std::vector<Texture*> shadowsMapCube){
    this->shadowsMapCube = shadowsMapCube;
}

void ObjectRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void ObjectRender::setLightRender(ObjectRender* lightRender){
    if (lightRender){
        if (!this->lightRender)
            this->lightRender = ObjectRender::newInstance();
        this->lightRender->properties = lightRender->properties;
    }
}

void ObjectRender::setFogRender(ObjectRender* fogRender) {
    if (fogRender){
        if (!this->fogRender)
            this->fogRender = ObjectRender::newInstance();
        this->fogRender->properties = fogRender->properties;
    }
}

void ObjectRender::setVertexSize(unsigned int vertexSize){
    this->vertexSize = vertexSize;
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

void ObjectRender::addVertexBuffer(std::string name, unsigned int size, void* data, bool dynamic){
    if (data && (size > 0))
        vertexBuffers[name] = { size, data, dynamic };
}

void ObjectRender::addVertexAttribute(int type, std::string buffer, unsigned int elements, unsigned int stride, size_t offset){
    if ((!buffer.empty()) && (elements > 0))
        vertexAttributes[type] = { buffer, elements, stride, offset};
}

void ObjectRender::addIndex(unsigned int size, void* data, bool dynamic){
    if (data && (size > 0))
        indexAttribute = { size, data, dynamic };
}

void ObjectRender::addProperty(int type, int datatype, unsigned int size, void* data){
    if (data && (size > 0))
        properties[type] = { datatype, size, data };
}

void ObjectRender::updateVertexBuffer(std::string name, unsigned int size, void* data){
    if (vertexBuffers.count(name))
        addVertexBuffer(name, size, data);
}

void ObjectRender::updateIndex(unsigned int size, void* data){
    if (indexAttribute.data)
        addIndex(size, data);
}

void ObjectRender::setNumLights(int numLights){
    this->numLights = numLights;
}

void ObjectRender::setNumShadows2D(int numShadows2D){
    this->numShadows2D = numShadows2D;
}

void ObjectRender::setNumShadowsCube(int numShadowsCube){
    this->numShadowsCube = numShadowsCube;
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
    if (lightRender == NULL || isSky){
        numLights = 0;
    }
}

void ObjectRender::checkFog(){
    if (fogRender != NULL){
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
    
        program->setDefinitions(numLights, numShadows2D, numShadowsCube, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText);
        program->load();
    }
}

bool ObjectRender::load(){
    
    loadProgram();
    
    if (texture)
        texture->load();

    for (int i = 0; i < shadowsMap2D.size(); i++)
        if (shadowsMap2D[i])
            shadowsMap2D[i]->load();

    for (int i = 0; i < shadowsMapCube.size(); i++)
        if (shadowsMapCube[i])
            shadowsMapCube[i]->load();
    
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

    if (numLights > 0){
        lightRender->setProgram(program);
        lightRender->load();
    }

    if (hasFog){
        fogRender->setProgram(program);
        fogRender->load();
    }
    
    return true;
}

bool ObjectRender::prepareDraw(){
    
    //lightRender and fogRender need to be called after main rander use program
    if (numLights > 0){
        lightRender->prepareDraw();
    }
    
    if (hasFog){
        fogRender->prepareDraw();
    }

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
