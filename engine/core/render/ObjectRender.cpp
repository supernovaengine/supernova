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

void ObjectRender::beginLoad(PrimitiveType primitiveType, bool depth){
    backend.beginLoad(primitiveType, depth);
}

void ObjectRender::loadIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset){
    backend.loadIndex(buffer, dataType, offset);
}

void ObjectRender::loadAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized){
    backend.loadAttribute(slotAttribute, buffer, elements, dataType, stride, offset, normalized);
}

void ObjectRender::loadShader(ShaderRender* shader){
    backend.loadShader(shader);
}

void ObjectRender::loadTexture(int slotTexture, ShaderStageType stage, TextureRender* texture){
    backend.loadTexture(slotTexture, stage, texture);
}

void ObjectRender::endLoad(){
    backend.endLoad();
}

void ObjectRender::beginDraw(){
    backend.beginDraw();
}

void ObjectRender::applyUniform(int slotUniform, ShaderStageType stage, UniformDataType datatype, unsigned int count, void* data){
    backend.applyUniform(slotUniform, stage, datatype, count, data);
}

void ObjectRender::draw(int vertexCount){
    backend.draw(vertexCount);
}

void ObjectRender::destroy(){
    backend.destroy();

}