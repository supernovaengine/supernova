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

bool BufferRender::createBuffer(unsigned int size, void* data, BufferType type, BufferUsage usage){
    return backend.createBuffer(size, data, type, usage);
}

void BufferRender::updateBuffer(unsigned int size, void* data){
    backend.updateBuffer(size, data);
}

void BufferRender::destroyBuffer(){
    backend.destroyBuffer();
}