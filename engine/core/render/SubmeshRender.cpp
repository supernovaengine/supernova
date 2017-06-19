#include "SubmeshRender.h"

#include "Submesh.h"
#include "Engine.h"
#include "render/gles2/GLES2Submesh.h"

using namespace Supernova;


SubmeshRender::SubmeshRender(){
    
}

SubmeshRender::~SubmeshRender(){
    
}

SubmeshRender* SubmeshRender::newInstance(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Submesh();
    }
    return NULL;
}

void SubmeshRender::fillSubmeshProperties(){
    if (submesh){
        indicesSizes = (unsigned int)submesh->getIndices()->size();
        
        if (submesh->getMaterial()->getTextures().size() > 0){
            textured = true;
        }else{
            textured = false;
        }
        
        material = submesh->getMaterial();
        indices = submesh->getIndices();
        isLoaded = submesh->isLoaded();
    }
}

void SubmeshRender::setSubmesh(Submesh* submesh){
    this->submesh = submesh;
}

bool SubmeshRender::load(){
    
    fillSubmeshProperties();
    
    return true;
}

bool SubmeshRender::draw(){
    
    fillSubmeshProperties();
    
    return true;
}

void SubmeshRender::destroy(){
    
    fillSubmeshProperties();

}
