//
// (c) 2019 Eduardo Doria.
//

#include "Attribute.h"

using namespace Supernova;

Attribute::Attribute(){
    setDataType(DataType::FLOAT);
    setBuffer("");
    setElements(0);
    setStride(0);
    setOffset(0);
    setCount(0);
}

Attribute::Attribute(DataType dataType, std::string bufferName, unsigned int elements, unsigned int stride, size_t offset){
    setDataType(dataType);
    setBuffer(bufferName);
    setElements(elements);
    setStride(stride);
    setOffset(offset);
    setCount(0);
}

Attribute::~Attribute(){

}

Attribute::Attribute(const Attribute& a){
    this->dataType = a.dataType;
    this->buffer = a.buffer;
    this->elements = a.elements;
    this->stride = a.stride;
    this->offset = a.offset;
    this->count = a.count;
}

Attribute& Attribute::operator = (const Attribute& a){
    this->dataType = a.dataType;
    this->buffer = a.buffer;
    this->elements = a.elements;
    this->stride = a.stride;
    this->offset = a.offset;
    this->count = a.count;

    return *this;
}

DataType Attribute::getDataType() const {
    return dataType;
}

void Attribute::setDataType(DataType dataType) {
    Attribute::dataType = dataType;
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

unsigned int Attribute::getCount() const {
    return count;
}

void Attribute::setCount(unsigned int count) {
    Attribute::count = count;
}
