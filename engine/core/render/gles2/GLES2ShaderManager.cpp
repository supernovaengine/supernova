#include "GLES2ShaderManager.h"

#include "platform/Log.h"
#include "GLES2Shaders.h"
#include "GLES2Util.h"
#include <stddef.h>
#include <stdlib.h>

std::vector<GLES2ShaderManager::ProgramStore> GLES2ShaderManager::programs;

GLES2ShaderManager::GLES2ShaderManager() {
    // TODO Auto-generated constructor stub

}

GLES2ShaderManager::~GLES2ShaderManager() {
    // TODO Auto-generated destructor stub
}

const char* GLES2ShaderManager::getVertexShader(const char* name){
    if (strcmp(name,"color")==0){
        return gVertexShaderColor;
    }else if (strcmp(name,"texture")==0){
        return gVertexShaderTexture;
    }else if (strcmp(name,"perfragment")==0){
        return gVertexShaderPerPixelLightTexture;
    }
    return NULL;

}

const char* GLES2ShaderManager::getFragmentShader(const char* name){
    if (strcmp(name,"color")==0){
        return gFragmentShaderColor;
    }else if (strcmp(name,"texture")==0){
        return gFragmentShaderTexture;
    }else if (strcmp(name,"perfragment")==0){
        return gFragmentShaderPerPixelLightTexture;
    }
    return NULL;
}


GLuint GLES2ShaderManager::loadShader(GLenum shaderType, const char* pSource) {
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

GLuint GLES2ShaderManager::createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }
    GLuint program = glCreateProgram();
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
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
    return program;
}

GLuint GLES2ShaderManager::useProgram(const char* programName){

    //Verify if there is a created program
    for (unsigned i=0; i<programs.size(); i++){
        if (strcmp(programs.at(i).key, programName)==0){
            programs.at(i).reference = programs.at(i).reference + 1;
            return programs.at(i).value;
        }
    }

    //If no create a new program
    GLuint program = createProgram(getVertexShader(programName), getFragmentShader(programName));
    programs.push_back((ProgramStore){program, programName, 1});
    return program;
}

void GLES2ShaderManager::detatchProgram(const char* programName){
    for (unsigned i=0; i<programs.size(); i++){
        if (programName!= NULL){
            if (strcmp(programs.at(i).key, programName)==0){
                programs.at(i).reference = programs.at(i).reference - 1;
                if (programs.at(i).reference == 0){
                    glDeleteProgram(programs.at(i).value);
                }
            }
        }
    }

}

void GLES2ShaderManager::clear(){
	programs.clear();
}
