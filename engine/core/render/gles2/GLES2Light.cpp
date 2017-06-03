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

    LightData* lightData = sceneRender->getLightData();

    glUniform3fv(u_AmbientLight, 1, lightData->ambientLight->ptr());

    glUniform1i(u_NumPointLight, lightData->numPointLight);
    if (lightData->numPointLight > 0){
        glUniform3fv(u_PointLightPos, lightData->numPointLight, &lightData->pointLightPos.front());
        glUniform1fv(u_PointLightPower, lightData->numPointLight, &lightData->pointLightPower.front());
        glUniform3fv(u_PointLightColor, lightData->numPointLight, &lightData->pointLightColor.front());
    }

    glUniform1i(u_NumSpotLight, lightData->numSpotLight);
    if (lightData->numSpotLight > 0){
        glUniform3fv(u_SpotLightPos, lightData->numSpotLight, &lightData->spotLightPos.front());
        glUniform1fv(u_SpotLightPower, lightData->numSpotLight, &lightData->spotLightPower.front());
        glUniform3fv(u_SpotLightColor, lightData->numSpotLight, &lightData->spotLightColor.front());
        glUniform3fv(u_SpotLightTarget, lightData->numSpotLight, &lightData->spotLightTarget.front());
        glUniform1fv(u_SpotLightCutOff, lightData->numSpotLight, &lightData->spotLightCutOff.front());
    }

    glUniform1i(u_NumDirectionalLight, lightData->numDirectionalLight);
    if (lightData->numDirectionalLight > 0){
        glUniform3fv(u_DirectionalLightDir, lightData->numDirectionalLight, &lightData->directionalLightDir.front());
        glUniform1fv(u_DirectionalLightPower, lightData->numDirectionalLight, &lightData->directionalLightPower.front());
        glUniform3fv(u_DirectionalLightColor, lightData->numDirectionalLight, &lightData->directionalLightColor.front());
    }
}
