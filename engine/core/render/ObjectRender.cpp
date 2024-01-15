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

void ObjectRender::addAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized){
    backend.addAttribute(slotAttribute, buffer, elements, dataType, stride, offset, normalized);
}

void ObjectRender::addShader(ShaderRender* shader){
    backend.addShader(shader);
}

void ObjectRender::addTexture(std::pair<int, int> slotTexture, ShaderStageType stage, TextureRender* texture){
    backend.addTexture(slotTexture, stage, texture);
}

void ObjectRender::endLoad(uint8_t pipelines){
    backend.endLoad(pipelines);
}

void ObjectRender::beginDraw(PipelineType pipType){
    backend.beginDraw(pipType);
}

void ObjectRender::applyUniformBlock(int slotUniform, ShaderStageType stage, unsigned int count, void* data){
    backend.applyUniformBlock(slotUniform, stage, count, data);
}

void ObjectRender::draw(int vertexCount){
    backend.draw(vertexCount);
}

void ObjectRender::destroy(){
    backend.destroy();

}