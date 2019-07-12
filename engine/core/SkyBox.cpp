

#include "SkyBox.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

SkyBox::SkyBox(): Mesh() {
    
    primitiveType = S_PRIMITIVE_TRIANGLES;

    buffers["vertices"] = &buffer;

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    //buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    //buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);

    Attribute* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
}

SkyBox::~SkyBox() {
}

void SkyBox::updateMVPMatrix(){
    
    if (this->viewProjectionMatrix != NULL){
        this->modelViewProjectionMatrix = (*this->viewProjectionMatrix);
    }
    
}

void SkyBox::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition) {

    skyViewMatrix = *viewMatrix;

    this->viewMatrix = &skyViewMatrix;
    this->viewMatrix->set(3,0,0);
    this->viewMatrix->set(3,1,0);
    this->viewMatrix->set(3,2,0);
    this->viewMatrix->set(3,3,1);
    this->viewMatrix->set(2,3,0);
    this->viewMatrix->set(1,3,0);
    this->viewMatrix->set(0,3,0);

    skyViewProjectionMatrix = (*projectionMatrix) * (*this->viewMatrix);

    this->viewProjectionMatrix = &skyViewProjectionMatrix;

    this->cameraPosition = *cameraPosition;

    updateMVPMatrix();
}

bool SkyBox::draw(){
    //Nothing to do
    //Scene will call draw()
    return true;
}

bool SkyBox::load(){
    if (this->submeshes[0]->getMaterial()->getTexture() == NULL)
        this->submeshes[0]->getMaterial()->setTextureCube(textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);

    instanciateRender();

    render->addProgramDef(S_PROGRAM_IS_SKY);

    return Mesh::load();
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
