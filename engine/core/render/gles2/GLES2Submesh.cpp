#include "GLES2Submesh.h"

#include "Material.h"
#include "Engine.h"
#include "GLES2Util.h"

using namespace Supernova;

GLES2Submesh::GLES2Submesh(){

    usageBuffer = GL_STATIC_DRAW;
    
}

GLES2Submesh::~GLES2Submesh(){
    
}

void GLES2Submesh::useIndicesBuffer(){
    if (indicesSizes > 0){
        
        if (indexBufferSize == 0){
            indexBuffer = GLES2Util::createVBO();
        }
        if (indexBufferSize >= indices->size()){
            GLES2Util::updateVBO(indexBuffer,GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), &indices->front());
        }else{
            indexBufferSize = std::max((unsigned int)indices->size(), minBufferSize);
            GLES2Util::dataVBO(indexBuffer, GL_ELEMENT_ARRAY_BUFFER, indexBufferSize * sizeof(unsigned int), &indices->front(), usageBuffer);
            
        }
    }
}

bool GLES2Submesh::load(){

    if (!SubmeshRender::load()){
        return false;
    }

    if (isDynamic){
        usageBuffer = GL_DYNAMIC_DRAW;
    }
    
    if (indicesSizes > 0){
        indexBufferSize = 0;
        useIndicesBuffer();
    }
    
    if (!textured){
        //Fix Chrome warnings of no texture bound with an empty texture
        if (Engine::getPlatform() == S_WEB){
            GLES2Util::generateEmptyTexture();
        }
    }
    
    return true;
}

void GLES2Submesh::destroy(){

    if (indicesSizes > 0)
        glDeleteBuffers(1, &indexBuffer);
    
    SubmeshRender::destroy();
}
