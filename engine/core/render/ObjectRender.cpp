#include "ObjectRender.h"

using namespace Supernova;


ObjectRender::ObjectRender(){

}


ObjectRender::~ObjectRender(){

}

void ObjectRender::useVertexProperty(int type, unsigned long size, void* data){
    vertexProperties[type] = { size, data };
}

void ObjectRender::updateVertexProperty(int type, unsigned long size){
    vertexProperties[type].size = size;
}

bool ObjectRender::load(){

}

bool ObjectRender::draw(){

}

void ObjectRender::destroy(){

}