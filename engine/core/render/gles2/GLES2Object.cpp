#include "GLES2Object.h"
#include "GLES2Util.h"

using namespace Supernova;


GLES2Object::GLES2Object(){

    usageBuffer = GL_STATIC_DRAW;
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadVertexProperty(int type, unsigned int count, unsigned long size, void* data){
    ObjectRender::loadVertexProperty(type, count, size, data);
    
    bufferProperty vp = bufferProperties[type];

    if (vp.size == 0){
        vp.buffer = GLES2Util::createVBO();
    }
    if (vp.size >= size){
        GLES2Util::updateVBO(vp.buffer, GL_ARRAY_BUFFER, size * count * sizeof(GLfloat), data);
    }else{
        vp.size = std::max((unsigned int)size, minBufferSize);
        GLES2Util::dataVBO(vp.buffer, GL_ARRAY_BUFFER, vp.size * count * sizeof(GLfloat), data, usageBuffer);
    }
}

void GLES2Object::loadIndex(int type, unsigned long size, void* data){
    ObjectRender::loadIndex(type, size, data);
    
    bufferProperty ib = bufferProperties[type];
    
    if (ib.size == 0){
        ib.buffer = GLES2Util::createVBO();
    }
    if (ib.size >= size){
        GLES2Util::updateVBO(ib.buffer,GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data);
    }else{
        ib.size = std::max((unsigned int)size, minBufferSize);
        GLES2Util::dataVBO(ib.buffer, GL_ELEMENT_ARRAY_BUFFER, ib.size * sizeof(unsigned int), data, usageBuffer);
        
    }
}

bool GLES2Object::load(){
    if (!ObjectRender::load()){
        return false;
    }

    return true;
}

bool GLES2Object::draw(){
    if (!ObjectRender::draw()){
        return false;
    }

    return true;
}

void GLES2Object::destroy(){

}
