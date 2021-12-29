//
// (c) 2019 Eduardo Doria.
//

#include "Attribute.h"

using namespace Supernova;

Attribute::Attribute(){
    setDataType(AttributeDataType::FLOAT);
    setBuffer("");
    setElements(0);
    setOffset(0);
    setCount(0);
    setNormalized(true);
}

Attribute::Attribute(AttributeDataType dataType, std::string bufferName, unsigned int elements, size_t offset, bool normalized){
    setDataType(dataType);
    setBuffer(bufferName);
    setElements(elements);
    setOffset(offset);
    setCount(0);
    setNormalized(normalized);
}

Attribute::~Attribute(){

}

Attribute::Attribute(const Attribute& a){
    this->dataType = a.dataType;
    this->buffer = a.buffer;
    this->elements = a.elements;
    this->offset = a.offset;
    this->count = a.count;
    this->normalized = a.normalized;
}

Attribute& Attribute::operator = (const Attribute& a){
    this->dataType = a.dataType;
    this->buffer = a.buffer;
    this->elements = a.elements;
    this->offset = a.offset;
    this->count = a.count;
    this->normalized = a.normalized;

    return *this;
}

AttributeDataType Attribute::getDataType() const {
    return dataType;
}

void Attribute::setDataType(AttributeDataType dataType) {
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

bool Attribute::getNormalized() const {
    return normalized;
}

void Attribute::setNormalized(bool normalized) {
    this->normalized = normalized;
}
