
#include "GLES2Program.h"

#include <algorithm>
#include <stdlib.h>
#include "shaders/GLES2Shaders.h"
#include "GLES2Util.h"
#include "Log.h"

using namespace Supernova;

std::string GLES2Program::getVertexShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gVertexMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gVertexPointsPerPixelLightShader;
    }else if (shaderType == S_SHADER_LINES){
        return gVertexLinesShader;
    }else if (shaderType == S_SHADER_DEPTH_RTT){
        return gVertexDepthShader;
    }
    return "";
}


std::string GLES2Program::getFragmentShader(int shaderType){
    if (shaderType == S_SHADER_MESH){
        return gFragmentMeshPerPixelLightShader;
    }else if (shaderType == S_SHADER_POINTS){
        return gFragmentPointsPerPixelLightShader;
    }else if (shaderType == S_SHADER_LINES){
        return gFragmentLinesShader;
    }else if (shaderType == S_SHADER_DEPTH_RTT){
        return gFragmentDepthShader;
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

void GLES2Program::createProgram(int shaderType, int programDefs, int numPointLights, int numSpotLights, int numDirLights, int numShadows2D, int numShadowsCube, int numBlendMapColors){
    ProgramRender::createProgram(shaderType, programDefs, numPointLights, numSpotLights, numDirLights, numShadows2D, numShadowsCube, numBlendMapColors);
    
    std::string shaderName = "";
    if (shaderType == S_SHADER_MESH){
        shaderName = "Mesh";
    }else if (shaderType == S_SHADER_POINTS){
        shaderName = "Points";
    }

    std::string definitions = "";

    this->numPointLights = std::min(MAXLIGHTS_GLES2, numPointLights);
    this->numSpotLights = std::min(MAXLIGHTS_GLES2 - numPointLights, numSpotLights);
    this->numDirLights = std::min(MAXLIGHTS_GLES2 - numPointLights - numSpotLights, numDirLights);
    this->numShadows2D = std::min(MAXSHADOWS_GLES2, numShadows2D);
    this->numShadowsCube = std::min(MAXSHADOWS_GLES2 - numShadows2D, numShadowsCube);
    this->numBlendMapColors = numBlendMapColors;

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
    if (programDefs & S_PROGRAM_USE_MORPHTARGET){
        definitions += "#define USE_MORPHTARGET\n";
    }
    if (programDefs & S_PROGRAM_USE_MORPHNORMAL){
        definitions += "#define USE_NORMAL\n";
        definitions += "#define USE_MORPHNORMAL\n";
    }
    if (programDefs & S_PROGRAM_IS_SKY){
        definitions += "#define IS_SKY\n";
    }
    if (programDefs & S_PROGRAM_IS_TEXT){
        definitions += "#define IS_TEXT\n";
    }
    if (programDefs & S_PROGRAM_IS_TERRAIN){
        definitions += "#define IS_TERRAIN\n";
    }
    if (this->numPointLights > 0 || this->numSpotLights > 0 || this->numDirLights > 0){
        definitions += "#define USE_NORMAL\n";
        definitions += "#define USE_LIGHTING\n";
    }
    if (this->numPointLights > 0){
        definitions += "#define USE_POINTLIGHT\n";
    }
    if (this->numSpotLights > 0){
        definitions += "#define USE_SPOTLIGHT\n";
    }
    if (this->numDirLights > 0){
        definitions += "#define USE_DIRLIGHT\n";
    }
    if (this->numShadows2D > 0){
        definitions += "#define USE_SHADOWS2D\n";
    }
    if (this->numShadowsCube > 0){
        definitions += "#define USE_SHADOWSCUBE\n";
    }
    
    std::string pVertexSource = definitions + getVertexShader(shaderType);
    std::string pFragmentSource = definitions + getFragmentShader(shaderType);

    if (this->numPointLights > 0 || this->numSpotLights > 0 || this->numDirLights > 0){
        pVertexSource = replaceAll(pVertexSource, "NUMPOINTLIGHTS", std::to_string(this->numPointLights));
        pFragmentSource = replaceAll(pFragmentSource, "NUMPOINTLIGHTS", std::to_string(this->numPointLights));

        pVertexSource = replaceAll(pVertexSource, "NUMSPOTLIGHTS", std::to_string(this->numSpotLights));
        pFragmentSource = replaceAll(pFragmentSource, "NUMSPOTLIGHTS", std::to_string(this->numSpotLights));

        pVertexSource = replaceAll(pVertexSource, "NUMDIRLIGHTS", std::to_string(this->numDirLights));
        pFragmentSource = replaceAll(pFragmentSource, "NUMDIRLIGHTS", std::to_string(this->numDirLights));

        pVertexSource = replaceAll(pVertexSource, "NUMSHADOWS2D", std::to_string(this->numShadows2D));
        pFragmentSource = replaceAll(pFragmentSource, "NUMSHADOWS2D", std::to_string(this->numShadows2D));

        pVertexSource = replaceAll(pVertexSource, "NUMSHADOWSCUBE", std::to_string(this->numShadowsCube));
        pFragmentSource = replaceAll(pFragmentSource, "NUMSHADOWSCUBE", std::to_string(this->numShadowsCube));
    }
    if (programDefs & S_PROGRAM_IS_TERRAIN){
        pVertexSource = replaceAll(pVertexSource, "NUMBLENDMAPCOLORS", std::to_string(this->numBlendMapColors));
        pFragmentSource = replaceAll(pFragmentSource, "NUMBLENDMAPCOLORS", std::to_string(this->numBlendMapColors));
    }
    if (programDefs & S_PROGRAM_USE_SKINNING){
        pVertexSource = replaceAll(pVertexSource, "MAXBONES", "70");
        pFragmentSource = replaceAll(pFragmentSource, "MAXBONES", "70");
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
