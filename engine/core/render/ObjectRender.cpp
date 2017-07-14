#include "ObjectRender.h"

using namespace Supernova;


ObjectRender::ObjectRender(){
    minBufferSize = 0;
}


ObjectRender::~ObjectRender(){

}

void ObjectRender::setMinBufferSize(unsigned int minBufferSize){
    this->minBufferSize = minBufferSize;
}

void ObjectRender::loadVertexAttribute(int type, unsigned int elements, unsigned long size, void* data){
    
}

void ObjectRender::loadIndex(unsigned long size, void* data){
    
}

void ObjectRender::setProperty(int type, int datatype, unsigned long size, void* data){
    
}

bool ObjectRender::load(){
    return true;
}

bool ObjectRender::draw(){
    return true;
}

void ObjectRender::destroy(){

}
