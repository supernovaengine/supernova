#include "Submesh.h"
#include "render/TextureManager.h"

Submesh::Submesh(){
    this->texture = "";
    this->loaded = false;
    this->distanceToCamera = -1;
    this->transparent = false;
}

Submesh::~Submesh(){

}

Submesh::Submesh(const Submesh& s){
    this->texture = s.texture;
    this->color = s.color;
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
    this->transparent = s.transparent;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->texture = s.texture;
    this->color = s.color;
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
    this->transparent = s.transparent;

    return *this;
}

void Submesh::setTexture(std::string texture){    
    this->texture = texture;
}

void Submesh::setColor(Vector4 color){

    if (color.w != 1){
        transparent = true;
    }

    this->color = color;
}

void Submesh::setIndices(std::vector<unsigned int> indices){
    this->indices = indices;
}

void Submesh::addIndex(unsigned int index){
    this->indices.push_back(index);
}

std::string Submesh::getTexture(){
    return texture;
}

Vector4* Submesh::getColor(){
    return &color;
}

std::vector<unsigned int>* Submesh::getIndices(){
    return &indices;
}

unsigned int Submesh::getIndex(int offset){
    return indices[offset];
}
