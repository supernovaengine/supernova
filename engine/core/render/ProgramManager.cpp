
#include "ProgramManager.h"
#include "Supernova.h"


std::vector<ProgramManager::ProgramStore> ProgramManager::programs;

ProgramRender* ProgramManager::getProgramRender(){
    if (Supernova::getRenderAPI() == S_GLES2){
        return new GLES2Program();
    }

    return NULL;
}

std::shared_ptr<ProgramRender> ProgramManager::useProgram(std::string shaderName, std::string definitions){

    //Verify if there is a created program
    for (unsigned i=0; i<programs.size(); i++){
        if (programs.at(i).key == shaderName + "|" + definitions){
            return programs.at(i).value;
        }
    }

    //If no create a new program
    ProgramRender* program = getProgramRender();
    program->createProgram(shaderName, definitions);
    std::shared_ptr<ProgramRender> shaderPtr(program);
    programs.push_back((ProgramStore){shaderPtr, shaderName + "|" + definitions});
    
    return programs.at(programs.size()-1).value;
}

void ProgramManager::deleteUnused(){
    
    int remove = findToRemove();
    while (remove >= 0){
        programs.erase(programs.begin() + remove);
        remove = findToRemove();
    }

    
}

int ProgramManager::findToRemove(){
    for (unsigned i=0; i<programs.size(); i++){
        if (programs.at(i).value.use_count() <= 1){
            programs.at(i).value.get()->deleteProgram();
            return i;
        }
    }
    return -1;
}

void ProgramManager::clear(){
    programs.clear();
}
