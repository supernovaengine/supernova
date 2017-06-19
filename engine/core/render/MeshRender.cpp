#include "MeshRender.h"
#include "PrimitiveMode.h"
#include "Mesh.h"
#include "Scene.h"

using namespace Supernova;

MeshRender::MeshRender(){

    lighting = false;
    hasfog = false;
    hasTextureRect = false;

    sceneRender = NULL;

    isLoaded = false;
    isSky = false;
    isDynamic = false;
    
    primitiveMode = 0;

}

MeshRender::~MeshRender(){
}

void MeshRender::setMesh(Mesh* mesh){
    this->mesh = mesh;
}

void MeshRender::updateVertices(){
    
}

void MeshRender::updateTexcoords(){
    
}

void MeshRender::updateNormals(){
    
}

void MeshRender::updateIndices(){

}

void MeshRender::checkLighting(){
    lighting = false;
    if ((sceneRender != NULL) && (!isSky)){
        lighting = sceneRender->lighting;
    }
}

void MeshRender::checkFog(){
    hasfog = false;
    if ((sceneRender != NULL) && (sceneRender->getFog() != NULL)){
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
    if (mesh){
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
        
        isLoaded = mesh->isLoaded();
        isSky = mesh->isSky();
        isDynamic = mesh->isDynamic();
        
        primitiveMode = mesh->getPrimitiveMode();
    }
}

bool MeshRender::load(){
    
    fillMeshProperties();

    if (vertices->size() <= 0){
        return false;
    }
    
    checkLighting();
    checkFog();
    checkTextureRect();

    return true;
}

bool MeshRender::draw() {
    
    fillMeshProperties();

    return true;
}

void MeshRender::destroy() {
    
    fillMeshProperties();
    
    isLoaded = false;
    
}
