#include "Mesh.h"
#include "Scene.h"
#include "render/TextureManager.h"
#include "platform/Log.h"

using namespace Supernova;

Mesh::Mesh(): ConcreteObject(){
    submeshes.push_back(new Submesh(&material));
    skymesh = false;
    dynamic = false;

    render = NULL;
}

Mesh::~Mesh(){
    destroy();
    removeAllSubmeshes();

    if (render)
        delete render;
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

bool Mesh::isDynamic(){
    return dynamic;
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

void Mesh::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    sortTransparentSubmeshes();
}

void Mesh::updateMatrix(){
    ConcreteObject::updateMatrix();
    
    this->normalMatrix = modelMatrix.getInverse().getTranspose();

    sortTransparentSubmeshes();
}

void Mesh::removeAllSubmeshes(){
    for (std::vector<Submesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::load(){
    
    while (vertices.size() > texcoords.size()){
        texcoords.push_back(Vector2(0,0));
    }
    
    while (vertices.size() > normals.size()){
        normals.push_back(Vector3(0,0,0));
    }

    MeshRender::newInstance(&render);
    
    render->setMesh(this);

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        submeshes[i]->load();
    }

    bool renderloaded = render->load();

    if (renderloaded)
        return ConcreteObject::load();
    else
        return false;
}

bool Mesh::draw(){
    if (!ConcreteObject::draw())
        return false;
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->draw();
    }

    return render->draw();
}

void Mesh::destroy(){
    ConcreteObject::destroy();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }
    
    render->destroy();
}
