#include "SubmeshRender.h"

#include "Submesh.h"
#include "Engine.h"
#include "render/gles2/GLES2Submesh.h"

using namespace Supernova;


SubmeshRender::SubmeshRender(){
    indicesSizes = 0;
    textured = false;
    material = NULL;

    isDynamic = false;

    minBufferSize = 0;
}

SubmeshRender::~SubmeshRender(){
    
}

void SubmeshRender::newInstance(SubmeshRender** render) {
    if (*render == NULL) {
        if (Engine::getRenderAPI() == S_GLES2) {
            *render = new GLES2Submesh();
        }
    }
}

void SubmeshRender::fillSubmeshProperties(){
    if (submesh){
        indicesSizes = (unsigned int)submesh->getIndices()->size();
        
        if (submesh->getMaterial()->getTexture()){
            textured = true;
        }else{
            textured = false;
        }
        
        material = submesh->getMaterial();
        indices = submesh->getIndices();

        isDynamic = submesh->isDynamic();

        minBufferSize = submesh->getMinBufferSize();
    }
}

void SubmeshRender::setSubmesh(Submesh* submesh){
    this->submesh = submesh;
}

bool SubmeshRender::load(){
    
    fillSubmeshProperties();
    
    if (textured){
        material->getTexture()->load();
    }
    
    return true;
}

bool SubmeshRender::draw(){
    
    fillSubmeshProperties();
    
    return true;
}

void SubmeshRender::destroy(){
    
    fillSubmeshProperties();
    
    if (textured){
        material->getTexture()->destroy();
    }

}
