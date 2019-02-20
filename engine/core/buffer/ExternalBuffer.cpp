//
// (c) 2019 Eduardo Doria.
//

#include "ExternalBuffer.h"

using namespace Supernova;

ExternalBuffer::ExternalBuffer(): Buffer(){

}

ExternalBuffer::~ExternalBuffer(){

}

void ExternalBuffer::setData(unsigned char* data, size_t size){
    this->data = data;
    this->size = size;
}