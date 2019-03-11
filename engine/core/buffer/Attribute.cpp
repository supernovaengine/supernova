//
// (c) 2019 Eduardo Doria.
//

#include "Attribute.h"

using namespace Supernova;

Attribute::Attribute(){

}

Attribute::Attribute(int type, std::string bufferName, unsigned int elements, unsigned int stride, size_t offset){
    setType(type);
    setBuffer(bufferName);
    setElements(elements);
    setStride(stride);
    setOffset(offset);
}

Attribute::~Attribute(){

}

int Attribute::getType() const {
    return type;
}

void Attribute::setType(int type) {
    Attribute::type = type;
}

const std::string &Attribute::getBuffer() const{
    return buffer;
}

void Attribute::setBuffer(const std::string &buffer){
    this->buffer = buffer;
}

unsigned int Attribute::getElements() const{
    return elements;
}

void Attribute::setElements(unsigned int elements){
    this->elements = elements;
}

unsigned int Attribute::getStride() const{
    return stride;
}

void Attribute::setStride(unsigned int stride){
    this->stride = stride;
}

const size_t &Attribute::getOffset() const{
    return offset;
}

void Attribute::setOffset(const size_t &offset){
    this->offset = offset;
}
