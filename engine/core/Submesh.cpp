#include "Submesh.h"
#include "render/TextureManager.h"

Submesh::Submesh(){
    this->loaded = false;
    this->distanceToCamera = -1;
    this->transparent = false;
    this->textureType = S_TEXTURE_2D;
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
    this->textureType = s.textureType;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->textures = s.textures;
    this->color = s.color;
    this->indices = s.indices;
    this->loaded = s.loaded;
    this->distanceToCamera = s.distanceToCamera;
    this->transparent = s.transparent;
    this->textureType = s.textureType;

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

void Submesh::setTextureType(int textureType){
    this->textureType = textureType;
}

void Submesh::setTextureCube(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down){
    this->textureType = S_TEXTURE_CUBE;
    textures.clear();
    textures.push_back(right);
    textures.push_back(left);
    textures.push_back(up);
    textures.push_back(down);
    textures.push_back(back);
    textures.push_back(front);
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

int Submesh::getTextureType(){
    return textureType;
}
