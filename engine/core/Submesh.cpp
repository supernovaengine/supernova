#include "Submesh.h"
#include "render/TextureManager.h"

using namespace Supernova;

Submesh::Submesh(){
    this->loaded = false;
    this->distanceToCamera = -1;
    this->material = NULL;
    this->newMaterial = false;
}

Submesh::Submesh(Material* material): Submesh() {
    this->material = material;
}

Submesh::~Submesh(){
    if (newMaterial)
        delete material;
}

Submesh::Submesh(const Submesh& s){
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;

    return *this;
}

void Submesh::setIndices(std::vector<unsigned int> indices){
    this->indices = indices;
}

void Submesh::addIndex(unsigned int index){
    this->indices.push_back(index);
}

std::vector<unsigned int>* Submesh::getIndices(){
    return &indices;
}

unsigned int Submesh::getIndex(int offset){
    return indices[offset];
}

void Submesh::createNewMaterial(){
    this->material = new Material();
    this->newMaterial = true;
}

void Submesh::setMaterial(Material* material){
    this->material = material;
}

Material* Submesh::getMaterial(){
    return this->material;
}
