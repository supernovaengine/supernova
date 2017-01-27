#include "Submesh.h"
#include "render/TextureManager.h"

Submesh::Submesh(){
    this->loaded = false;
    this->distanceToCamera = -1;
    this->transparent = false;
}

Submesh::~Submesh(){

}

Submesh::Submesh(const Submesh& s){
    this->textures = s.textures;
    this->color = s.color;
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
    this->transparent = s.transparent;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->textures = s.textures;
    this->color = s.color;
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
    this->transparent = s.transparent;

    return *this;
}

void Submesh::setTexture(std::string texture){
    if (textures.size() == 0){
        textures.push_back(texture);
    }else{
        textures[0] = texture;
    }
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

std::vector<std::string> Submesh::getTextures(){
    return textures;
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
