#include "GLES2Object.h"
#include "GLES2Util.h"
#include "GLES2Program.h"

using namespace Supernova;


GLES2Object::GLES2Object(){

    usageBuffer = GL_STATIC_DRAW;
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadVertexAttribute(int type, attributeData att){
    
    bufferData vp = attributeBuffers[type];

    if (vp.size == 0){
        vp.buffer = GLES2Util::createVBO();
    }
    if (vp.size >= att.size){
        GLES2Util::updateVBO(vp.buffer, GL_ARRAY_BUFFER, att.size * att.elements * sizeof(GLfloat), att.data);
    }else{
        vp.size = std::max((unsigned int)att.size, minBufferSize);
        GLES2Util::dataVBO(vp.buffer, GL_ARRAY_BUFFER, vp.size * att.elements * sizeof(GLfloat), att.data, usageBuffer);
    }
}

void GLES2Object::loadIndex(attributeData att){
    
    bufferData ib = indexBuffer;
    
    if (ib.size == 0){
        ib.buffer = GLES2Util::createVBO();
    }
    if (ib.size >= att.size){
        GLES2Util::updateVBO(ib.buffer,GL_ELEMENT_ARRAY_BUFFER, att.size * sizeof(unsigned int), att.data);
    }else{
        ib.size = std::max((unsigned int)att.size, minBufferSize);
        GLES2Util::dataVBO(ib.buffer, GL_ELEMENT_ARRAY_BUFFER, ib.size * sizeof(unsigned int), att.data, usageBuffer);
        
    }
}

void GLES2Object::useProperty(int type, propertyData prop){
    
    GLuint pb = propertyBuffers[type];
    
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
    
}

bool GLES2Object::load(){
    if (!ObjectRender::load()){
        return false;
    }
    
    std::string programName = "points_perfragment";
    std::string programDefs = "";
    //if (lighting){
        programDefs += "#define USE_LIGHTING\n";
    //}
    //if (hasfog){
    //    programDefs += "#define HAS_FOG\n";
    //}
    //if (hasTextureRect){
        programDefs += "#define HAS_TEXTURERECT\n";
    //}
    
    program.setShader(programName);
    program.setDefinitions(programDefs);
    program.load();
    
    GLuint glesProgram = ((GLES2Program*)program.getProgramRender().get())->getProgram();
    
    //light.setProgram((GLES2Program*)gProgram.getProgramRender().get());
    //fog.setProgram((GLES2Program*)gProgram.getProgramRender().get());
    
    //useTexture = glGetUniformLocation(glesProgram, "uUseTexture");
    
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        int type = it->first;
        
        std::string attribName;
        
        if (type == S_VERTEXATTRIBUTE_VERTICES){
            attribName = "a_Position";
        }else if (type == S_VERTEXATTRIBUTE_NORMALS){
            attribName = "a_Normal";
        }else if (type == S_VERTEXATTRIBUTE_POINTSIZES){
            attribName = "a_PointSize";
        }else if (type == S_VERTEXATTRIBUTE_POINTCOLORS){
            attribName = "a_pointColor";
        }else if (type == S_VERTEXATTRIBUTE_TEXTURERECTS){
            attribName = "a_textureRect";            
        }
        
        loadVertexAttribute(type, it->second);
        attributeBuffers[type].handle = glGetAttribLocation(glesProgram, attribName.c_str());
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
