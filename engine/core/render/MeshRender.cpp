#include "MeshRender.h"
#include "PrimitiveMode.h"
#include "Mesh.h"
#include "Scene.h"


MeshRender::MeshRender(){

    loaded = false;
    lighting = false;
    hasfog = false;
    
    submeshesGles.clear();

    sceneRender = NULL;

    isSky = false;
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
    primitiveMode = mesh->getPrimitiveMode();
}

bool MeshRender::load(){

    if (mesh->getVertices()->size() <= 0){
        return false;
    }
    
    fillMeshProperties();
    
    checkLighting();
    checkFog();

    submeshesGles.clear();
    //---> For meshes
    for (unsigned int i = 0; i < submeshes->size(); i++){
        submeshesGles[submeshes->at(i)].indicesSizes = (int)submeshes->at(i)->getIndices()->size();
        submeshesGles[submeshes->at(i)].textureRect = submeshes->at(i)->getMaterial()->getTextureRect();

        if (submeshes->at(i)->getMaterial()->getTextures().size() > 0){
            submeshesGles[submeshes->at(i)].textured = true;
        }else{
            submeshesGles[submeshes->at(i)].textured = false;
        }
        if (submeshes->at(i)->getMaterial()->getTextureType() == S_TEXTURE_2D &&  texcoords->size() == 0){
            submeshesGles[submeshes->at(i)].textured = false;
        }
    }

    loaded = true;

    return true;
}

bool MeshRender::draw() {
    
    fillMeshProperties();

    return true;
}
