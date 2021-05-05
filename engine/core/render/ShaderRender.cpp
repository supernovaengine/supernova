#include "ShaderRender.h"

using namespace Supernova;

ShaderRender::ShaderRender(){ }

ShaderRender::ShaderRender(const ShaderRender& rhs) : backend(rhs.backend) { }

ShaderRender& ShaderRender::operator=(const ShaderRender& rhs) { 
    backend = rhs.backend; 
    return *this; 
}

ShaderRender::~ShaderRender(){
    //Cannot destroy because its a handle
}

bool ShaderRender::createShader(ShaderData& shaderData){
    return backend.createShader(shaderData);
}

void ShaderRender::destroyShader(){
    backend.destroyShader();
}