
#include "ShaderManager.h"
#include "Supernova.h"


std::vector<ShaderManager::ShaderStore> ShaderManager::shaders;

ShaderRender* ShaderManager::getShaderRender(){
    if (Supernova::getRenderAPI() == S_GLES2){
        return new GLES2Program();
    }
    
    return NULL;
}

ShaderRender* ShaderManager::useShader(std::string shaderName){
    
    //Verify if there is a created program
    for (unsigned i=0; i<shaders.size(); i++){
        if (shaders.at(i).key == shaderName){
            shaders.at(i).reference = shaders.at(i).reference + 1;
            return shaders.at(i).value;
        }
    }
    
    //If no create a new program
    ShaderRender* shader = getShaderRender();
    shader->createShader(shaderName);
    shaders.push_back((ShaderStore){shader, shaderName, 1});
    return shader;
}

void ShaderManager::deleteShader(std::string shaderName){
    int remove = -1;
    
    for (unsigned i=0; i<shaders.size(); i++){
        if (shaders.at(i).key == shaderName){
            shaders.at(i).reference = shaders.at(i).reference - 1;
            if (shaders.at(i).reference == 0){
                remove = i;
                shaders.at(i).value->deleteShader();
            }
        }
    }
    
    if (remove > 0)
        shaders.erase(shaders.begin() + remove);
    
}

void ShaderManager::clear(){
    shaders.clear();
}
