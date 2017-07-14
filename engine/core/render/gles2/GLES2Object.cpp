#include "GLES2Object.h"
#include "GLES2Util.h"

using namespace Supernova;


GLES2Object::GLES2Object(){

    usageBuffer = GL_STATIC_DRAW;
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadVertexAttribute(int type, unsigned int elements, unsigned long size, void* data){
    ObjectRender::loadVertexAttribute(type, elements, size, data);
    
    bufferData vp = vertexAttributes[type];

    if (vp.size == 0){
        vp.buffer = GLES2Util::createVBO();
    }
    if (vp.size >= size){
        GLES2Util::updateVBO(vp.buffer, GL_ARRAY_BUFFER, size * elements * sizeof(GLfloat), data);
    }else{
        vp.size = std::max((unsigned int)size, minBufferSize);
        GLES2Util::dataVBO(vp.buffer, GL_ARRAY_BUFFER, vp.size * elements * sizeof(GLfloat), data, usageBuffer);
    }
}

void GLES2Object::loadIndex(unsigned long size, void* data){
    ObjectRender::loadIndex(size, data);
    
    bufferData ib = indexAttribute;
    
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

void GLES2Object::setProperty(int type, int datatype, unsigned long size, void* data){
    ObjectRender::setProperty(type, datatype, size, data);
    
    GLuint pr = properties[type];
    
    if (datatype == S_PROPERTYDATA_FLOAT1){
        glUniform1fv(pr, (GLsizei)size, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_FLOAT2){
        glUniform2fv(pr, (GLsizei)size, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_FLOAT3){
        glUniform3fv(pr, (GLsizei)size, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_FLOAT4){
        glUniform4fv(pr, (GLsizei)size, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_INT1){
        glUniform1iv(pr, (GLsizei)size, (GLint*)data);
    }else if (datatype == S_PROPERTYDATA_INT2){
        glUniform2iv(pr, (GLsizei)size, (GLint*)data);
    }else if (datatype == S_PROPERTYDATA_INT3){
        glUniform3iv(pr, (GLsizei)size, (GLint*)data);
    }else if (datatype == S_PROPERTYDATA_INT4){
        glUniform4iv(pr, (GLsizei)size, (GLint*)data);
    }else if (datatype == S_PROPERTYDATA_MATRIX2){
        glUniformMatrix2fv(pr, (GLsizei)size, GL_FALSE, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_MATRIX3){
        glUniformMatrix3fv(pr, (GLsizei)size, GL_FALSE, (GLfloat*)data);
    }else if (datatype == S_PROPERTYDATA_MATRIX4){
        glUniformMatrix4fv(pr, (GLsizei)size, GL_FALSE, (GLfloat*)data);
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
