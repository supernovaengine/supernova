//
// (c) 2018 Eduardo Doria.
//

#include "VertexBuffer.h"
#include "Log.h"

using namespace Supernova;

VertexBuffer::VertexBuffer(){
    blockSize = 0;
}

VertexBuffer::~VertexBuffer(){

}

void VertexBuffer::addAttribute(std::string name, int elements, int offset){
    if (buffer.size() == 0) {
        if (attributes.count(name) == 0) {
            AttributeData attData;
            attData.count = 0;
            attData.elements = elements;
            attData.offset = offset;

            attributes[name] = attData;

            blockSize += elements;
        } else{
            Log::Error("Attribute %s already exists", name.c_str());
        }
    }else{
        Log::Error("Cannot add attribute with not cleared buffer");
    }
}

AttributeData* VertexBuffer::getAttribute(std::string name){
    if (attributes.count(name) > 0){
        return &attributes[name];
    }

    return NULL;
}

void VertexBuffer::clearAll(){
    blockSize = 0;
    attributes.clear();
    clearBuffer();
}

void VertexBuffer::clearBuffer(){
    vertexSize = 0;
    buffer.clear();
}

void VertexBuffer::addValue(AttributeData* attribute, Vector2 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void VertexBuffer::addValue(std::string attributeName, Vector2 vector){
    addValue(getAttribute(attributeName), vector);
}

void VertexBuffer::addValue(AttributeData* attribute, Vector3 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void VertexBuffer::addValue(std::string attributeName, Vector3 vector){
    addValue(getAttribute(attributeName), vector);
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, Vector2 vector){
    setValue(index, attribute, 2, &vector[0]);
}
void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, Vector3 vector){
    setValue(index, attribute, 3, &vector[0]);
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, float* vector){
    if (attribute){
        if (attribute->elements == numValues) {

            if (index > attribute->count)
                attribute->count = index;

            unsigned pos = (index * blockSize) + attribute->offset;

            if ((attribute->count*blockSize) > buffer.size())
                buffer.resize(attribute->count*blockSize);

            for (int i = 0; i < numValues; i++) {
                buffer[pos++] = vector[i];
            }

            if (attribute->count > vertexSize)
                vertexSize = attribute->count;

        }else{
            Log::Error("Error add value to attribute, elements number is incorrect");
        }
    }else{
        Log::Error("Error add value, attribute not exist");
    }
}

float VertexBuffer::getValue(AttributeData* attribute, unsigned int index, int elementIndex){
    if (elementIndex >= 0 && elementIndex < attribute->elements) {
        unsigned pos = (index * blockSize) + attribute->offset + elementIndex;
        if ((pos+1) <= buffer.size()){
            return buffer[pos];
        }else{
            Log::Error("Attribute index is bigger than buffer");
        }
    }else{
        Log::Error("Element index is not correct");
    }

    return 0;
}

float* VertexBuffer::getBuffer(){
    return &buffer[0];
}

unsigned int VertexBuffer::getSize(){
    return buffer.size();
}

unsigned int VertexBuffer::getBlockSize(){
    return blockSize;
}

unsigned int VertexBuffer::getVertexSize(){
    return vertexSize;
}
