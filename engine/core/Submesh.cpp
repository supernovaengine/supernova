#include "Submesh.h"


Submesh::Submesh(){

}

Submesh::~Submesh(){

}

Submesh::Submesh(const Submesh& s){
    this->texture = s.texture;
    this->color = s.color;
    this->indices = s.indices;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->texture = s.texture;
    this->color = s.color;
    this->indices = s.indices;

    return *this;
}

void Submesh::setTexture(Texture texture){
    this->texture = texture;
}

void Submesh::setColor(Vector4 color){
    this->color = color;
}

void Submesh::setIndices(std::vector<unsigned int> indices){
    this->indices = indices;
}

void Submesh::addIndex(unsigned int index){
    this->indices.push_back(index);
}

Texture* Submesh::getTexture(){
    return &texture;
}

Vector4* Submesh::getColor(){
    return &color;
}

std::vector<unsigned int>* Submesh::getIndices(){
    return &indices;
}
