#include "GLES2Fog.h"

using namespace Supernova;

GLES2Fog::GLES2Fog(){
    program = NULL;
}

GLES2Fog::~GLES2Fog(){
    
}

void GLES2Fog::setProgram(GLES2Program* program){
    this->program = program;
}

void GLES2Fog::getUniformLocations(){
    u_fogMode = glGetUniformLocation(program->getProgram(), "u_fogMode");
    u_fogColor = glGetUniformLocation(program->getProgram(), "u_fogColor");
    u_fogDensity = glGetUniformLocation(program->getProgram(), "u_fogDensity");
    u_fogVisibility = glGetUniformLocation(program->getProgram(), "u_fogVisibility");
    u_fogStart = glGetUniformLocation(program->getProgram(), "u_fogStart");
    u_fogEnd = glGetUniformLocation(program->getProgram(), "u_fogEnd");
}

void GLES2Fog::setUniformValues(SceneRender* sceneRender){
    glUniform1i(u_fogMode, sceneRender->getFog()->getMode());
    glUniform3fv(u_fogColor, 1, sceneRender->getFog()->getColor().ptr());
    glUniform1f(u_fogDensity, sceneRender->getFog()->getDensity());
    glUniform1f(u_fogVisibility, sceneRender->getFog()->getVisibility());
    glUniform1f(u_fogStart, sceneRender->getFog()->getLinearStart());
    glUniform1f(u_fogEnd, sceneRender->getFog()->getLinearEnd());
}
