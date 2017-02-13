

#include "SkyBox.h"


#include "platform/Log.h"
#include "PrimitiveMode.h"

SkyBox::SkyBox(): Mesh() {
    
    primitiveMode = S_TRIANGLES;
    objectType = S_DRAW_SKY;
    
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f,  1.0f));
    vertices.push_back(Vector3(-1.0f,  1.0f, -1.0f));
    
    vertices.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    vertices.push_back(Vector3(-1.0f, -1.0f,  1.0f));
    vertices.push_back(Vector3(1.0f, -1.0f,  1.0f));
}

SkyBox::~SkyBox() {
}

void SkyBox::updateMatrices(){
    if (this->viewProjectionMatrix != NULL){
        this->modelViewProjectionMatrix = (*this->viewProjectionMatrix);
    }
}

void SkyBox::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition) {

    skyViewMatrix = *viewMatrix;

    this->viewMatrix = &skyViewMatrix;
    this->viewMatrix->set(3,0,0);
    this->viewMatrix->set(3,1,0);
    this->viewMatrix->set(3,2,0);
    this->viewMatrix->set(3,3,1);
    this->viewMatrix->set(2,3,0);
    this->viewMatrix->set(1,3,0);
    this->viewMatrix->set(0,3,0);

    skyViewProjectionMatrix = (*this->viewMatrix) * (*projectionMatrix);

    this->viewProjectionMatrix = &skyViewProjectionMatrix;

    this->cameraPosition = cameraPosition;

    updateMatrices();
}

bool SkyBox::load(){
    this->submeshes[0]->getMaterial()->setTextureCube(textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);
    Mesh::load();
    return true;
}

bool SkyBox::draw(){
    return true;
}


void SkyBox::setTextureFront(std::string texture){
    this->textureFront = texture;
}

void SkyBox::setTextureBack(std::string texture){
    this->textureBack = texture;
}

void SkyBox::setTextureLeft(std::string texture){
    this->textureLeft = texture;
}

void SkyBox::setTextureRight(std::string texture){
    this->textureRight = texture;
}

void SkyBox::setTextureUp(std::string texture){
    this->textureUp = texture;
}

void SkyBox::setTextureDown(std::string texture){
    this->textureDown = texture;
}
