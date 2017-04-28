#include "MeshRender.h"
#include "PrimitiveMode.h"
#include "Mesh.h"


MeshRender::MeshRender(){

    loaded = false;
    lighting = false;
    hasfog = false;
    
    submeshesGles.clear();

    sceneRender = NULL;

    isSky = false;
    //isRectImage = false;

    textureRect = NULL;

}

MeshRender::~MeshRender(){
}

void MeshRender::setMesh(Mesh* mesh){
    this->mesh = mesh;
}

void MeshRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void MeshRender::setIsSky(bool isSky){
    this->isSky = isSky;
}
/*
void MeshRender::setIsRectImage(bool isRectImage){
    this->isRectImage = isRectImage;
}

void MeshRender::setTextureRect(TextureRect* textureRect){
    this->textureRect = textureRect;
}
*/
void MeshRender::checkLighting(){
    lighting = false;
    if ((sceneRender != NULL) && (!isSky)){
        lighting = sceneRender->lighting;
    }
}

void MeshRender::checkFog(){
    hasfog = false;
    if ((sceneRender != NULL) && (sceneRender->fog != NULL)){
        hasfog = true;
    }
}

bool MeshRender::load(){

    if (mesh->getVertices().size() <= 0){
        return false;
    }

    checkLighting();
    checkFog();

    submeshesGles.clear();
    //---> For meshes
    for (unsigned int i = 0; i < mesh->getSubmeshes().size(); i++){
        submeshesGles[mesh->getSubmeshes()[i]].indicesSizes = (int)mesh->getSubmeshes()[i]->getIndices()->size();

        if (mesh->getSubmeshes()[i]->getMaterial()->getTextures().size() > 0){
            submeshesGles[mesh->getSubmeshes()[i]].textured = true;
        }else{
            submeshesGles[mesh->getSubmeshes()[i]].textured = false;
        }
        if (mesh->getSubmeshes()[i]->getMaterial()->getTextureType() == S_TEXTURE_2D &&  mesh->getTexcoords().size() == 0){
            submeshesGles[mesh->getSubmeshes()[i]].textured = false;
        }
    }

    loaded = true;

    return true;
}

bool MeshRender::draw() {

    return true;
}
