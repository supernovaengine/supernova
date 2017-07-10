#include "GLES2Object.h"

using namespace Supernova;


GLES2Object::GLES2Object(){

}

GLES2Object::~GLES2Object(){

}

void GLES2Object::updateVertexProperty(int type, unsigned long size){
    ObjectRender::updateVertexProperty(type, size);

}

bool GLES2Object::load(){
    ObjectRender::load();

}

bool GLES2Object::draw(){
    ObjectRender::draw();

}

void GLES2Object::destroy(){

}