//
// (c) 2018 Eduardo Doria.
//

#include "VertexBuffer.h"
#include "Log.h"
#include <cstdlib>

using namespace Supernova;

VertexBuffer::VertexBuffer(){

    const int len = 3;
    std::string randName;
    randName.resize(len);

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        randName[i] = alphanum[std::rand() % (sizeof(alphanum) - 1)];
    }

    name = "buffer|" + randName;

    blockSize = 0;
}

VertexBuffer::~VertexBuffer(){

}

void VertexBuffer::addAttribute(int attribute, int elements){
    if (buffer.size() == 0) {
        AttributeData attData;
        attData.count = 0;
        attData.elements = elements;
        attData.offset = blockSize;

        attributes[attribute] = attData;

        blockSize += elements;
    }else{
        Log::Error("Cannot add attribute with not cleared buffer");
    }
}

AttributeData* VertexBuffer::getAttribute(int attribute){
    if (attributes.count(attribute) > 0){
        return &attributes[attribute];
    }

    return NULL;
}

void VertexBuffer::clearAll(){
    blockSize = 0;
    attributes.clear();
    clearBuffer();
}

void VertexBuffer::clearBuffer(){
    for (auto x : attributes) {
        x.second.count = 0;
    }
    vertexSize = 0;
    buffer.clear();
}

void VertexBuffer::addValue(int attribute, float value){
    addValue(getAttribute(attribute), value);
}

void VertexBuffer::addValue(int attribute, Vector2 vector){
    addValue(getAttribute(attribute), vector);
}

void VertexBuffer::addValue(int attribute, Vector3 vector){
    addValue(getAttribute(attribute), vector);
}

void VertexBuffer::addValue(int attribute, Vector4 vector){
    addValue(getAttribute(attribute), vector);
}

void VertexBuffer::addValue(AttributeData* attribute, float value){
    if (attribute){
        setValue(attribute->count++, attribute, value);
    }
}

void VertexBuffer::addValue(AttributeData* attribute, Vector2 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void VertexBuffer::addValue(AttributeData* attribute, Vector3 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void VertexBuffer::addValue(AttributeData* attribute, Vector4 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, float value){
    setValue(index, attribute, 1, &value);
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, Vector2 vector){
    setValue(index, attribute, 2, &vector[0]);
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, Vector3 vector){
    setValue(index, attribute, 3, &vector[0]);
}

void VertexBuffer::setValue(unsigned int index, AttributeData* attribute, Vector4 vector){
    setValue(index, attribute, 4, &vector[0]);
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

Vector2 VertexBuffer::getValueVector2(int attribute, unsigned int index){
    return getValueVector2(getAttribute(attribute), index);
}

Vector3 VertexBuffer::getValueVector3(int attribute, unsigned int index){
    return getValueVector3(getAttribute(attribute), index);
}

Vector4 VertexBuffer::getValueVector4(int attribute, unsigned int index){
    return getValueVector4(getAttribute(attribute), index);
}

Vector2 VertexBuffer::getValueVector2(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * blockSize) + attribute->offset;
    if ((pos+2) <= buffer.size()){
        return Vector2(buffer[pos], buffer[pos+1]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector2();
}

Vector3 VertexBuffer::getValueVector3(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * blockSize) + attribute->offset;
    if ((pos+3) <= buffer.size()){
        return Vector3(buffer[pos], buffer[pos+1], buffer[pos+2]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector3();
}

Vector4 VertexBuffer::getValueVector4(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * blockSize) + attribute->offset;
    if ((pos+4) <= buffer.size()){
        return Vector4(buffer[pos], buffer[pos+1], buffer[pos+2], buffer[pos+3]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector4();
}

float VertexBuffer::getValue(int attribute, unsigned int index){
    return getValue(getAttribute(attribute), index, 0);
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

std::map<int, AttributeData> VertexBuffer::getAttributes(){
    return attributes;
};

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

const std::string &VertexBuffer::getName() const {
    return name;
}

void VertexBuffer::setName(const std::string &name) {
    VertexBuffer::name = name;
}
