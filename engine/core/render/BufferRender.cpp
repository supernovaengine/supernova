#include "BufferRender.h"

using namespace Supernova;

BufferRender::BufferRender(){ }

BufferRender::BufferRender(const BufferRender& rhs) : backend(rhs.backend) { }

BufferRender& BufferRender::operator=(const BufferRender& rhs) { 
    backend = rhs.backend; 
    return *this; 
}

BufferRender::~BufferRender(){
    //Cannot destroy because its a handle
}

bool BufferRender::createBuffer(unsigned int size, void* data, BufferType type, bool dynamic){
    return backend.createBuffer(size, data, type, dynamic);
}

void BufferRender::destroyBuffer(){
    backend.destroyBuffer();
}