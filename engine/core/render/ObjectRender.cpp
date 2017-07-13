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

void ObjectRender::loadVertexProperty(int type, unsigned int count, unsigned long size, void* data){
    
}

void ObjectRender::loadIndex(int type, unsigned long size, void* data){
    
}

bool ObjectRender::load(){
    return true;
}

bool ObjectRender::draw(){
    return true;
}

void ObjectRender::destroy(){

}
