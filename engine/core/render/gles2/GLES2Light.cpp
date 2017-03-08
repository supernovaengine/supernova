#include "GLES2Light.h"


GLES2Light::GLES2Light(){
    program = NULL;
}

GLES2Light::~GLES2Light(){

}

void GLES2Light::setProgram(GLES2Program* program){
    this->program = program;
}

void GLES2Light::getUniformLocations(){
    u_AmbientLight = glGetUniformLocation(program->getProgram(), "u_AmbientLight");

    u_NumPointLight = glGetUniformLocation(program->getProgram(), "u_NumPointLight");
    u_PointLightPos = glGetUniformLocation(program->getProgram(), "u_PointLightPos");
    u_PointLightPower = glGetUniformLocation(program->getProgram(), "u_PointLightPower");
    u_PointLightColor = glGetUniformLocation(program->getProgram(), "u_PointLightColor");

    u_NumSpotLight = glGetUniformLocation(program->getProgram(), "u_NumSpotLight");
    u_SpotLightPos = glGetUniformLocation(program->getProgram(), "u_SpotLightPos");
    u_SpotLightPower = glGetUniformLocation(program->getProgram(), "u_SpotLightPower");
    u_SpotLightColor = glGetUniformLocation(program->getProgram(), "u_SpotLightColor");
    u_SpotLightTarget = glGetUniformLocation(program->getProgram(), "u_SpotLightTarget");
    u_SpotLightCutOff = glGetUniformLocation(program->getProgram(), "u_SpotLightCutOff");

    u_NumDirectionalLight = glGetUniformLocation(program->getProgram(), "u_NumDirectionalLight");
    u_DirectionalLightDir = glGetUniformLocation(program->getProgram(), "u_DirectionalLightDir");
    u_DirectionalLightPower = glGetUniformLocation(program->getProgram(), "u_DirectionalLightPower");
    u_DirectionalLightColor = glGetUniformLocation(program->getProgram(), "u_DirectionalLightColor");
}

void GLES2Light::setUniformValues(SceneRender* sceneRender){
    glUniform3fv(u_AmbientLight, 1, sceneRender->ambientLight.ptr());

    glUniform1i(u_NumPointLight, sceneRender->numPointLight);
    if (sceneRender->numPointLight > 0){
        glUniform3fv(u_PointLightPos, sceneRender->numPointLight, &sceneRender->pointLightPos.front());
        glUniform1fv(u_PointLightPower, sceneRender->numPointLight, &sceneRender->pointLightPower.front());
        glUniform3fv(u_PointLightColor, sceneRender->numPointLight, &sceneRender->pointLightColor.front());
    }

    glUniform1i(u_NumSpotLight, sceneRender->numSpotLight);
    if (sceneRender->numSpotLight > 0){
        glUniform3fv(u_SpotLightPos, sceneRender->numSpotLight, &sceneRender->spotLightPos.front());
        glUniform1fv(u_SpotLightPower, sceneRender->numSpotLight, &sceneRender->spotLightPower.front());
        glUniform3fv(u_SpotLightColor, sceneRender->numSpotLight, &sceneRender->spotLightColor.front());
        glUniform3fv(u_SpotLightTarget, sceneRender->numSpotLight, &sceneRender->spotLightTarget.front());
        glUniform1fv(u_SpotLightCutOff, sceneRender->numSpotLight, &sceneRender->spotLightCutOff.front());
    }

    glUniform1i(u_NumDirectionalLight, sceneRender->numDirectionalLight);
    if (sceneRender->numDirectionalLight > 0){
        glUniform3fv(u_DirectionalLightDir, sceneRender->numDirectionalLight, &sceneRender->directionalLightDir.front());
        glUniform1fv(u_DirectionalLightPower, sceneRender->numDirectionalLight, &sceneRender->directionalLightPower.front());
        glUniform3fv(u_DirectionalLightColor, sceneRender->numDirectionalLight, &sceneRender->directionalLightColor.front());
    }
}