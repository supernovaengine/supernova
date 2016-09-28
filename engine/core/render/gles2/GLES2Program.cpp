
#include "GLES2Program.h"

#include <stdlib.h>
#include "GLES2Shaders.h"
#include "GLES2Util.h"
#include "platform/Log.h"


const char* GLES2Program::getVertexShader(const char* name){
    if (strcmp(name,"color")==0){
        return gVertexShaderColor;
    }else if (strcmp(name,"texture")==0){
        return gVertexShaderTexture;
    }else if (strcmp(name,"perfragment")==0){
        return gVertexShaderPerPixelLightTexture;
    }
    return NULL;
    
}

const char* GLES2Program::getFragmentShader(const char* name){
    if (strcmp(name,"color")==0){
        return gFragmentShaderColor;
    }else if (strcmp(name,"texture")==0){
        return gFragmentShaderTexture;
    }else if (strcmp(name,"perfragment")==0){
        return gFragmentShaderPerPixelLightTexture;
    }
    return NULL;
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

void GLES2Program::createShader(std::string shaderName) {
    
    const char* pVertexSource = getVertexShader(shaderName.c_str());
    const char* pFragmentSource = getFragmentShader(shaderName.c_str());

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        Log::Error(LOG_TAG,"Could not load vertex shader: %s\n", shaderName.c_str());
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
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
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
}

void GLES2Program::deleteShader(){
    glDeleteProgram(program);
}



GLuint GLES2Program::getShader(){
    return program;
}
