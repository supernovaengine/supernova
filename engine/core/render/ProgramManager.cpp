
#include "ProgramManager.h"
#include "Supernova.h"


std::unordered_map<std::string, std::shared_ptr<ProgramRender>> ProgramManager::programs;

ProgramRender* ProgramManager::getProgramRender(){
    if (Supernova::getRenderAPI() == S_GLES2){
        return new GLES2Program();
    }

    return NULL;
}

std::shared_ptr<ProgramRender> ProgramManager::useProgram(std::string shaderName, std::string definitions){

    std::string key = shaderName + "|" + definitions;

    //Verify if there is a created program
    if (programs[key]){
        return programs[key];
    }

    //If no create a new program
    ProgramRender* program = getProgramRender();
    program->createProgram(shaderName, definitions);
    std::shared_ptr<ProgramRender> shaderPtr(program);

    programs[key] = shaderPtr;

    return programs[key];
}

void ProgramManager::deleteUnused(){

    ProgramManager::it_type remove = findToRemove();
    while (remove != programs.end()){
        programs.erase(remove);
        remove = findToRemove();
    }

}

ProgramManager::it_type ProgramManager::findToRemove(){

    for(ProgramManager::it_type iterator = programs.begin(); iterator != programs.end(); iterator++) {
        if (iterator->second.use_count() <= 1){
            iterator->second.get()->deleteProgram();
            return iterator;
        }
    }

    return programs.end();
}

void ProgramManager::clear(){
    programs.clear();
}
