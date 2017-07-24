#include "GLES2Object.h"
#include "GLES2Util.h"
#include "GLES2Program.h"
#include "GLES2Texture.h"
#include "Engine.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_OFFSET(i) ((void*)(i))

using namespace Supernova;


GLES2Object::GLES2Object(){

    usageBuffer = GL_STATIC_DRAW;
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadVertexAttribute(int type, attributeData att){
    
    attributeGlData vp = attributeBuffers[type];

    if (vp.size == 0){
        vp.buffer = GLES2Util::createVBO();
    }
    if (vp.size >= att.size){
        GLES2Util::updateVBO(vp.buffer, GL_ARRAY_BUFFER, att.size * att.elements * sizeof(GLfloat), att.data);
    }else{
        vp.size = std::max((unsigned int)att.size, minBufferSize);
        GLES2Util::dataVBO(vp.buffer, GL_ARRAY_BUFFER, vp.size * att.elements * sizeof(GLfloat), att.data, usageBuffer);
    }

    attributeBuffers[type] = vp;
}

void GLES2Object::loadIndex(attributeData att){
    
    attributeGlData ib = indexBuffer;
    
    if (ib.size == 0){
        ib.buffer = GLES2Util::createVBO();
    }
    if (ib.size >= att.size){
        GLES2Util::updateVBO(ib.buffer,GL_ELEMENT_ARRAY_BUFFER, att.size * sizeof(unsigned int), att.data);
    }else{
        ib.size = std::max((unsigned int)att.size, minBufferSize);
        GLES2Util::dataVBO(ib.buffer, GL_ELEMENT_ARRAY_BUFFER, ib.size * sizeof(unsigned int), att.data, usageBuffer);
    }

    indexBuffer = ib;
}

void GLES2Object::useProperty(int type, propertyData prop){
    
    GLuint pb = propertyHandle[type];
    
    if (prop.datatype == S_PROPERTYDATA_FLOAT1){
        glUniform1fv(pb, (GLsizei)prop.size, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_FLOAT2){
        glUniform2fv(pb, (GLsizei)prop.size, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_FLOAT3){
        glUniform3fv(pb, (GLsizei)prop.size, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_FLOAT4){
        glUniform4fv(pb, (GLsizei)prop.size, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_INT1){
        glUniform1iv(pb, (GLsizei)prop.size, (GLint*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_INT2){
        glUniform2iv(pb, (GLsizei)prop.size, (GLint*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_INT3){
        glUniform3iv(pb, (GLsizei)prop.size, (GLint*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_INT4){
        glUniform4iv(pb, (GLsizei)prop.size, (GLint*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_MATRIX2){
        glUniformMatrix2fv(pb, (GLsizei)prop.size, GL_FALSE, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_MATRIX3){
        glUniformMatrix3fv(pb, (GLsizei)prop.size, GL_FALSE, (GLfloat*)prop.data);
    }else if (prop.datatype == S_PROPERTYDATA_MATRIX4){
        glUniformMatrix4fv(pb, (GLsizei)prop.size, GL_FALSE, (GLfloat*)prop.data);
    }

    propertyHandle[type] = pb;
    
}

void GLES2Object::updateVertexAttribute(int type, unsigned long size){
    ObjectRender::updateVertexAttribute(type, size);
    
    loadVertexAttribute(type, vertexAttributes[type]);
}

bool GLES2Object::load(){
    if (!ObjectRender::load()){
        return false;
    }
    
    attributeBuffers.clear();
    propertyHandle.clear();
    
    if (dynamicBuffer)
        usageBuffer = GL_DYNAMIC_DRAW;
    
    GLuint glesProgram = ((GLES2Program*)program->getProgramRender().get())->getProgram();
    
    light.setProgram((GLES2Program*)program->getProgramRender().get());
    fog.setProgram((GLES2Program*)program->getProgramRender().get());
    
    useTexture = glGetUniformLocation(glesProgram, "uUseTexture");
    
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        int type = it->first;
        
        std::string attribName;
        
        if (type == S_VERTEXATTRIBUTE_VERTICES){
            attribName = "a_Position";
        }else if (type == S_VERTEXATTRIBUTE_NORMALS){
            attribName = "a_Normal";
        }else if (type == S_VERTEXATTRIBUTE_POINTSIZES){
            attribName = "a_pointSize";
        }else if (type == S_VERTEXATTRIBUTE_POINTCOLORS){
            attribName = "a_pointColor";
        }else if (type == S_VERTEXATTRIBUTE_TEXTURERECTS){
            attribName = "a_textureRect";            
        }
        
        loadVertexAttribute(type, it->second);
        attributeBuffers[type].handle = glGetAttribLocation(glesProgram, attribName.c_str());
    }
    
    if (indexAttribute.data)
        loadIndex(indexAttribute);
    
    if (texture) {
        uTextureUnitLocation = glGetUniformLocation(glesProgram, "u_TextureUnit");
    }else{
        if (Engine::getPlatform() == S_WEB){
            GLES2Util::generateEmptyTexture();
            uTextureUnitLocation = glGetUniformLocation(glesProgram, "u_TextureUnit");
        }
    }
    
    for (std::unordered_map<int, propertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        int type = it->first;
        
        std::string propertyName;
        
        if (type == S_PROPERTY_MVPMATRIX){
            propertyName = "u_mvpMatrix";
        }else if (type == S_PROPERTY_MODELMATRIX){
            propertyName = "u_mMatrix";
        }else if (type == S_PROPERTY_NORMALMATRIX){
            propertyName = "u_nMatrix";
        }else if (type == S_PROPERTY_CAMERAPOS){
            propertyName = "u_EyePos";
        }
        
        propertyHandle[type] = glGetUniformLocation(glesProgram, propertyName.c_str());
    }
    
    if (hasLight){
        light.getUniformLocations();
    }
    
    if (hasFog){
        fog.getUniformLocations();
    }
    
    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Object::draw(){
    if (!ObjectRender::draw()){
        return false;
    }
    
    GLuint glesProgram = ((GLES2Program*)program->getProgramRender().get())->getProgram();
    
    glUseProgram(glesProgram);
    GLES2Util::checkGlError("glUseProgram");
    
    for (std::unordered_map<int, propertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        useProperty(it->first, it->second);
    }
    
    if (hasLight){
        light.setUniformValues(sceneRender);
    }
    
    if (hasFog){
        fog.setUniformValues(sceneRender);
    }
    
    int attributePos = 0;
    
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        attributeGlData att = attributeBuffers[it->first];
        
        glEnableVertexAttribArray(attributePos);
        glBindBuffer(GL_ARRAY_BUFFER, att.buffer);
        if (att.handle == -1) att.handle = attributePos;
        glVertexAttribPointer(att.handle, it->second.elements, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(0));
    
        attributePos++;
    }
    
    if (indexAttribute.data){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.buffer);
    }
    
    glUniform1i(useTexture, (texture?true:false));
    
    if (texture){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(((GLES2Texture*)(texture->getTextureRender().get()))->getTextureType(),
                      ((GLES2Texture*)(texture->getTextureRender().get()))->getTexture());
        glUniform1i(uTextureUnitLocation, 0);
    }else{
        if (Engine::getPlatform() == S_WEB){
            //Fix Chrome warnings of no texture bound
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
            glUniform1i(uTextureUnitLocation, 0);
        }
    }
    
    GLenum modeGles = GL_TRIANGLES;
    if (primitiveType == S_PRIMITIVE_TRIANGLES_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }
    if (primitiveType == S_PRIMITIVE_POINTS){
        modeGles = GL_POINTS;
    }
    
    if (indexAttribute.data){
        glDrawElements(modeGles, (GLsizei)indexAttribute.size, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    }else{
        glDrawArrays(modeGles, 0, (GLsizei)vertexAttributes[S_VERTEXATTRIBUTE_VERTICES].size);
    }
    
    for (int i = 0; i <= (attributePos-1); i++)
        glDisableVertexAttribArray(i);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    GLES2Util::checkGlError("Error on draw GLES2");

    return true;
}

void GLES2Object::destroy(){

}
