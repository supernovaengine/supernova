
#include "GLES2Program.h"

#include <algorithm>
#include <stdlib.h>
#include "GLES2Shaders.h"
#include "GLES2Util.h"
#include "Log.h"

using namespace Supernova;

std::string GLES2Program::getVertexShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gVertexMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gVertexPointsPerPixelLightShader;
    }else if (shaderType == S_SHADER_DEPTH_RTT){
        return gVertexDepthRTTShader;
    }
    return "";
}


std::string GLES2Program::getFragmentShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gFragmentMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gFragmentPointsPerPixelLightShader;
    }else if (shaderType == S_SHADER_DEPTH_RTT){
        return gFragmentDepthRTTShader;
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
                    Log::Error("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void GLES2Program::createProgram(int shaderType, int programDefs, int numLights, int numShadows2D, int numShadowsCube){
    ProgramRender::createProgram(shaderType, programDefs, numLights, numShadows2D, numShadowsCube);
    
    std::string shaderName = "";
    if (shaderType == S_SHADER_MESH){
        shaderName = "Mesh";
    }else if (shaderType == S_SHADER_POINTS){
        shaderName = "Points";
    }

    std::string definitions = "";

    maxLights = std::min(MAXLIGHTS_GLES2, numLights);
    maxShadows2D = std::min(MAXSHADOWS_GLES2, numShadows2D);
    maxShadowsCube = std::min(MAXSHADOWS_GLES2 - numShadows2D, numShadowsCube);

    if (programDefs & S_PROGRAM_USE_FOG){
        definitions += "#define HAS_FOG\n";
    }
    if (programDefs & S_PROGRAM_USE_TEXCOORD){
        definitions += "#define USE_TEXTURECOORDS\n";
    }
    if (programDefs & S_PROGRAM_USE_TEXRECT){
        definitions += "#define HAS_TEXTURERECT\n";
    }
    if (programDefs & S_PROGRAM_USE_TEXCUBE){
        definitions += "#define USE_TEXTURECUBE\n";
    }
    if (programDefs & S_PROGRAM_USE_SKINNING){
        definitions += "#define HAS_SKINNING\n";
    }
    if (programDefs & S_PROGRAM_IS_SKY){
        definitions += "#define IS_SKY\n";
    }
    if (programDefs & S_PROGRAM_IS_TEXT){
        definitions += "#define IS_TEXT\n";
    }
    if (numLights > 0){
        definitions += "#define USE_LIGHTING\n";
    }
    if (maxShadows2D > 0){
        definitions += "#define HAS_SHADOWS2D\n";
    }
    if (maxShadowsCube > 0){
        definitions += "#define HAS_SHADOWSCUBE\n";
    }
    
    std::string pVertexSource = definitions + getVertexShader(shaderType);
    std::string pFragmentSource = definitions + getFragmentShader(shaderType);

    if (numLights > 0){
        pVertexSource = replaceAll(pVertexSource, "MAXLIGHTS", std::to_string(maxLights));
        pFragmentSource = replaceAll(pFragmentSource, "MAXLIGHTS", std::to_string(maxLights));

        pVertexSource = replaceAll(pVertexSource, "MAXSHADOWS2D", std::to_string(maxShadows2D));
        pFragmentSource = replaceAll(pFragmentSource, "MAXSHADOWS2D", std::to_string(maxShadows2D));

        pVertexSource = replaceAll(pVertexSource, "MAXSHADOWSCUBE", std::to_string(maxShadowsCube));
        pFragmentSource = replaceAll(pFragmentSource, "MAXSHADOWSCUBE", std::to_string(maxShadowsCube));

        pVertexSource = replaceAll(pVertexSource, "MAXCASCADES", std::to_string(MAXCASCADES_GLES2));
        pFragmentSource = replaceAll(pFragmentSource, "MAXCASCADES", std::to_string(MAXCASCADES_GLES2));
    }
    if (programDefs & S_PROGRAM_USE_SKINNING){
        pVertexSource = replaceAll(pVertexSource, "MAXBONES", "68");
        pFragmentSource = replaceAll(pFragmentSource, "MAXBONES", "68");
    }

    pFragmentSource = unrollLoops(pFragmentSource);

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource.c_str());
    if (!vertexShader) {
        Log::Error("Could not load vertex shader: %s\n", shaderName.c_str());
    }
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource.c_str());
    if (!pixelShader) {
        Log::Error("Could not load fragment shader: %s\n", shaderName.c_str());
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
                    Log::Error("Could not link program:\n%s\n", buf);
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
