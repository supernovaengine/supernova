#include "MeshRender.h"
#include "PrimitiveMode.h"
#include "Mesh.h"
#include "Scene.h"


MeshRender::MeshRender(){

    loaded = false;
    lighting = false;
    hasfog = false;
    hasTextureRect = false;
    
    submeshesRender.clear();

    sceneRender = NULL;

    isSky = false;
    isDynamic = false;
    primitiveMode = 0;

}

MeshRender::~MeshRender(){
}

void MeshRender::setMesh(Mesh* mesh){
    this->mesh = mesh;
}

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

void MeshRender::checkTextureRect(){
    hasTextureRect = false;
    for (unsigned int i = 0; i < submeshes->size(); i++){
        if (submeshes->at(i)->getMaterial()->getTextureRect()){
            hasTextureRect = true;
        }
    }
}

void MeshRender::fillMeshProperties(){
    if (mesh->getScene() != NULL)
        sceneRender = mesh->getScene()->getSceneRender();
    
    vertices = mesh->getVertices();
    texcoords = mesh->getTexcoords();
    normals = mesh->getNormals();
    submeshes = mesh->getSubmeshes();
    
    modelViewProjectMatrix = mesh->getModelViewProjectMatrix();
    modelMatrix = mesh->getModelMatrix();
    normalMatrix = mesh->getNormalMatrix();
    cameraPosition = mesh->getCameraPosition();
    
    isSky = mesh->isSky();
    isDynamic = mesh->isDynamic();
    primitiveMode = mesh->getPrimitiveMode();
}

bool MeshRender::load(){
    
    fillMeshProperties();

    if (vertices->size() <= 0){
        return false;
    }
    
    checkLighting();
    checkFog();
    checkTextureRect();

    submeshesRender.clear();
    for (unsigned int i = 0; i < submeshes->size(); i++){
        submeshesRender[submeshes->at(i)].indicesSizes = (int)submeshes->at(i)->getIndices()->size();

        if (submeshes->at(i)->getMaterial()->getTextures().size() > 0){
            submeshesRender[submeshes->at(i)].textured = true;
        }else{
            submeshesRender[submeshes->at(i)].textured = false;
        }
        if (submeshes->at(i)->getMaterial()->getTextureType() == S_TEXTURE_2D &&  texcoords->size() == 0){
            submeshesRender[submeshes->at(i)].textured = false;
        }
    }

    loaded = true;

    return true;
}

bool MeshRender::draw() {
    
    fillMeshProperties();

    return true;
}
