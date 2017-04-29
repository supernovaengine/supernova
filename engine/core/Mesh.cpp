#include "Mesh.h"
#include "Scene.h"
#include "render/TextureManager.h"
#include "platform/Log.h"

Mesh::Mesh(): ConcreteObject(){
    submeshes.push_back(new Submesh(&material));
    skymesh = false;
}

Mesh::~Mesh(){
    destroy();
}

std::vector<Vector3>* Mesh::getVertices(){
    return &vertices;
}

std::vector<Vector3>* Mesh::getNormals(){
    return &normals;
}

std::vector<Vector2>* Mesh::getTexcoords(){
    return &texcoords;
}

std::vector<Submesh*>* Mesh::getSubmeshes(){
    return &submeshes;
}

bool Mesh::isSky(){
    return skymesh;
}

int Mesh::getPrimitiveMode(){
    return primitiveMode;
}

void Mesh::setTexcoords(std::vector<Vector2> texcoords){
    this->texcoords.clear();
    this->texcoords = texcoords;
}

void Mesh::setPrimitiveMode(int primitiveMode){
    this->primitiveMode = primitiveMode;
}

void Mesh::addVertex(Vector3 vertex){
    vertices.push_back(vertex);
}

void Mesh::addNormal(Vector3 normal){
    normals.push_back(normal);
}

void Mesh::addTexcoord(Vector2 texcoord){
    texcoords.push_back(texcoord);
}

void Mesh::addSubmesh(Submesh* submesh){
    submeshes.push_back(submesh);
}

void Mesh::sortTransparentSubmeshes(){

    if (transparent){
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (this->submeshes[i]->getIndices()->size() > 0){
                Vector3 submeshFirstVertice = vertices[this->submeshes[i]->getIndex(0)];
                submeshFirstVertice = modelMatrix * submeshFirstVertice;
                
                if (this->cameraPosition != NULL && this->submeshes[i]->getMaterial()->transparent){
                    this->submeshes[i]->distanceToCamera = ((*this->cameraPosition) - submeshFirstVertice).length();
                }
            }
        }
        
        if (this->cameraPosition != NULL){
            std::sort(submeshes.begin(), submeshes.end(),
                      [](const Submesh* a, const Submesh* b) -> bool
                      {
                          if (a->distanceToCamera == -1)
                              return true;
                          if (b->distanceToCamera == -1)
                              return false;
                          return a->distanceToCamera > b->distanceToCamera;
                      });
            
        }
    }

}

void Mesh::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::transform(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    sortTransparentSubmeshes();
}

void Mesh::update(){
    ConcreteObject::update();

    this->normalMatrix = modelMatrix.getInverse().getTranspose();

    sortTransparentSubmeshes();
}

bool Mesh::render(){
    return renderManager.draw();
}

void Mesh::removeAllSubmeshes(){
    for (std::vector<Submesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::load(){

    if (scene != NULL){
        //renderManager.getRender()->setSceneRender(((Scene*)scene)->getSceneRender());
    }
    
    while (vertices.size() > texcoords.size()){
        texcoords.push_back(Vector2(0,0));
    }
    
    while (vertices.size() > normals.size()){
        normals.push_back(Vector3(0,0,0));
    }
    
    renderManager.getRender()->setMesh(this);

    renderManager.load();

    return ConcreteObject::load();
}

bool Mesh::draw(){

    return ConcreteObject::draw();
}

void Mesh::destroy(){
    ConcreteObject::destroy();
    
    renderManager.destroy();
    
    removeAllSubmeshes();
}
