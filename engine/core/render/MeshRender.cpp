#include "MeshRender.h"
#include "PrimitiveMode.h"
#include "Mesh.h"
#include "Scene.h"
#include "Engine.h"
#include "render/gles2/GLES2Mesh.h"

using namespace Supernova;

MeshRender::MeshRender(){

    lighting = false;
    hasfog = false;
    hasTextureRect = false;
    hasTexture = false;
    hasTextureCube = false;

    sceneRender = NULL;

    isSky = false;
    isText = false;
    isDynamic = false;
    
    primitiveMode = 0;

    minBufferSize = 0;

}

MeshRender::~MeshRender(){

}

void MeshRender::newInstance(MeshRender** render){
    if (*render == NULL){
        if (Engine::getRenderAPI() == S_GLES2){
            *render = new GLES2Mesh();
        }
    }
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

void MeshRender::checkSubmeshProperties(){
    hasTextureRect = false;
    hasTexture = false;
    hasTextureCube = false;
    for (unsigned int i = 0; i < submeshes->size(); i++){
        if (submeshes->at(i)->getMaterial()->getTextureRect()){
            hasTextureRect = true;
        }
        if (submeshes->at(i)->getMaterial()->getTextures().size() > 0){
            hasTexture = true;
        }
        if (submeshes->at(i)->getMaterial()->getTextureType() == S_TEXTURE_CUBE){
            hasTextureCube = true;
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

        isSky = mesh->isSky();
        isText = mesh->isText();
        isDynamic = mesh->isDynamic();
        
        primitiveMode = mesh->getPrimitiveMode();

        minBufferSize = mesh->getMinBufferSize();
    }
}

bool MeshRender::load(){
    
    fillMeshProperties();

    if (vertices->size() <= 0){
        return false;
    }
    
    checkLighting();
    checkFog();
    checkSubmeshProperties();

    return true;
}

bool MeshRender::draw() {
    
    fillMeshProperties();

    return true;
}

void MeshRender::destroy() {
    
    fillMeshProperties();
    
}
