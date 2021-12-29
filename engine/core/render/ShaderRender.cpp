#include "ShaderRender.h"

using namespace Supernova;

ShaderRender::ShaderRender(){ }

ShaderRender::ShaderRender(const ShaderRender& rhs) : backend(rhs.backend), shaderData(rhs.shaderData) { }

ShaderRender& ShaderRender::operator=(const ShaderRender& rhs) { 
    backend = rhs.backend;
    shaderData = rhs.shaderData;
    return *this; 
}

ShaderRender::~ShaderRender(){
    //Cannot destroy because its a handle
}

bool ShaderRender::createShader(ShaderData& shaderData){
    this->shaderData = shaderData;

    //Keep only reflection info
    this->shaderData.releaseSourceData();
    
    return backend.createShader(shaderData);
}

void ShaderRender::destroyShader(){
    backend.destroyShader();
}

bool ShaderRender::isCreated(){
    return backend.isCreated();
}