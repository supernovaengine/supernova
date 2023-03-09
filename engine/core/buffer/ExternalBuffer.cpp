//
// (c) 2019 Eduardo Doria.
//

#include "ExternalBuffer.h"

using namespace Supernova;

ExternalBuffer::ExternalBuffer(): Buffer(){
    name = "";
}

ExternalBuffer::~ExternalBuffer(){

}

void ExternalBuffer::setData(unsigned char* data, size_t size){
    this->data = data;
    this->size = size;
}

void ExternalBuffer::setName(std::string name){
    this->name = name;
}

std::string ExternalBuffer::getName() const{
    return this->name;
}