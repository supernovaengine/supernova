//
// (c) 2024 Eduardo Doria.
//

#include "ObjectRender.h"

using namespace Supernova;

ObjectRender::ObjectRender(){ }

ObjectRender::ObjectRender(const ObjectRender& rhs) : backend(rhs.backend) { }

ObjectRender& ObjectRender::operator=(const ObjectRender& rhs) { 
    backend = rhs.backend; 
    return *this; 
}

ObjectRender::~ObjectRender(){
    //Cannot destroy because its a handle
}

void ObjectRender::beginLoad(PrimitiveType primitiveType){
    backend.beginLoad(primitiveType);
}

void ObjectRender::addIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset){
    backend.addIndex(buffer, dataType, offset);
}

void ObjectRender::addAttribute(int slot, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized){
    backend.addAttribute(slot, buffer, elements, dataType, stride, offset, normalized);
}

void ObjectRender::addStorageBuffer(int slot, ShaderStageType stage, BufferRender* buffer){
    backend.addStorageBuffer(slot, stage, buffer);
}

void ObjectRender::addShader(ShaderRender* shader){
    backend.addShader(shader);
}

void ObjectRender::addTexture(std::pair<int, int> slot, ShaderStageType stage, TextureRender* texture){
    backend.addTexture(slot, stage, texture);
}

bool ObjectRender::endLoad(uint8_t pipelines){
    return backend.endLoad(pipelines);
}

bool ObjectRender::beginDraw(PipelineType pipType){
    return backend.beginDraw(pipType);
}

void ObjectRender::applyUniformBlock(int slot, ShaderStageType stage, unsigned int count, void* data){
    backend.applyUniformBlock(slot, stage, count, data);
}

void ObjectRender::draw(int vertexCount){
    backend.draw(vertexCount);
}

void ObjectRender::destroy(){
    backend.destroy();

}