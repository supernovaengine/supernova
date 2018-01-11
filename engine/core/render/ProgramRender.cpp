#include "ProgramRender.h"

#include "gles2/GLES2Program.h"
#include "Engine.h"
#include "Scene.h"

using namespace Supernova;

std::unordered_map< std::string, std::shared_ptr<ProgramRender> > ProgramRender::programsRender;


ProgramRender::ProgramRender(){
    this->loaded = false;
}

ProgramRender::~ProgramRender(){
    
}

std::shared_ptr<ProgramRender> ProgramRender::sharedInstance(std::string id){
    if (programsRender.count(id) > 0){
        return programsRender[id];
    }else{
        
        if (Engine::getRenderAPI() == S_GLES2){
            programsRender[id] = std::shared_ptr<ProgramRender>(new GLES2Program());
            return programsRender[id];
        }
    }
    
    return NULL;
}

bool ProgramRender::isLoaded(){
    return loaded;
}

void ProgramRender::deleteUnused(){
    
    ProgramRender::it_type remove = findToRemove();
    while (remove != programsRender.end()){
        programsRender.erase(remove);
        remove = findToRemove();
    }
    
}

ProgramRender::it_type ProgramRender::findToRemove(){
    
    for(ProgramRender::it_type iterator = programsRender.begin(); iterator != programsRender.end(); iterator++) {
        if (iterator->second.use_count() <= 1){
            if (iterator->second.get() != NULL)
                iterator->second.get()->deleteProgram();
            return iterator;
        }
    }
    
    return programsRender.end();
}

int ProgramRender::getMaxShadows2D(){
    return maxShadows2D;
}

int ProgramRender::getMaxShadowsCube(){
    return maxShadowsCube;
}

void ProgramRender::createProgram(int shaderType, bool hasLight, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText, bool hasShadows2D, bool hasShadowsCube){
    loaded = true;

    maxLights = (int)Engine::getScene()->getLights()->size();
    maxShadows2D = Engine::getScene()->getLightData()->numShadows2D;
    maxShadowsCube = Engine::getScene()->getLightData()->numShadowsCube;
}

void ProgramRender::deleteProgram(){
    loaded = false;
}
