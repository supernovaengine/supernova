
#include "GLES2Program.h"

#include <stdlib.h>
#include "GLES2Shaders.h"
#include "GLES2Util.h"
#include "platform/Log.h"
#include "Program.h"


using namespace Supernova;

std::string GLES2Program::getVertexShader(std::string name){
    if (name == "mesh_perfragment"){
        return gVertexMeshPerPixelLightShader;
    }else if (name == "points_perfragment"){
        return gVertexPointsPerPixelLightShader;
    }
    return "";
}

std::string GLES2Program::getFragmentShader(std::string name){
    if (name == "mesh_perfragment"){
        return gFragmentMeshPerPixelLightShader;
    }else if (name == "points_perfragment"){
        return gFragmentPointsPerPixelLightShader;
    }
    return "";
}

std::string GLES2Program::getVertexShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gVertexMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gVertexPointsPerPixelLightShader;
    }
    return "";
}


std::string GLES2Program::getFragmentShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gFragmentMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gFragmentPointsPerPixelLightShader;
    }
    return "";
}


GLuint GLES2Program::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    Log::Error(LOG_TAG,"Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void GLES2Program::createProgram(std::string shaderName, std::string definitions) {
    ProgramRender::createProgram(shaderName, definitions);
    
    std::string pVertexSource = definitions + getVertexShader(shaderName);
    std::string pFragmentSource = definitions + getFragmentShader(shaderName);

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource.c_str());
    if (!vertexShader) {
        Log::Error(LOG_TAG,"Could not load vertex shader: %s\n", shaderName.c_str());
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource.c_str());
    if (!pixelShader) {
        Log::Error(LOG_TAG,"Could not load fragment shader: %s\n", shaderName.c_str());
    }
    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        GLES2Util::checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        GLES2Util::checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    Log::Error(LOG_TAG,"Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    //free(pVertexSource);
    //free(pFragmentSource);
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
}

void GLES2Program::createProgram(int shaderType, bool hasLight, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText){
    ProgramRender::createProgram(shaderType, hasLight, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText);
    
    std::string shaderName = "";
    if (shaderType == S_SHADER_MESH){
        shaderName = "Mesh";
    }else if (shaderType == S_SHADER_POINTS){
        shaderName = "Points";
    }
    
    std::string definitions = "";
    if (hasLight){
        definitions += "#define USE_LIGHTING\n";
    }
    if (hasFog){
        definitions += "#define HAS_FOG\n";
    }
    if (hasTextureCoords){
        definitions += "#define USE_TEXTURECOORDS\n";
    }
    if (hasTextureRect){
        definitions += "#define HAS_TEXTURERECT\n";
    }
    if (hasTextureCube){
        definitions += "#define USE_TEXTURECUBE\n";
    }
    if (isSky){
        definitions += "#define IS_SKY\n";
    }
    if (isText){
        definitions += "#define IS_TEXT\n";
    }
    
    std::string pVertexSource = definitions + getVertexShader(shaderType);
    std::string pFragmentSource = definitions + getFragmentShader(shaderType);
    
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource.c_str());
    if (!vertexShader) {
        Log::Error(LOG_TAG,"Could not load vertex shader: %s\n", shaderName.c_str());
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource.c_str());
    if (!pixelShader) {
        Log::Error(LOG_TAG,"Could not load fragment shader: %s\n", shaderName.c_str());
    }
    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        GLES2Util::checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        GLES2Util::checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    Log::Error(LOG_TAG,"Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    //free(pVertexSource);
    //free(pFragmentSource);
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
}

void GLES2Program::deleteProgram(){
    glDeleteProgram(program);
    ProgramRender::deleteProgram();
}



GLuint GLES2Program::getProgram(){
    return program;
}
