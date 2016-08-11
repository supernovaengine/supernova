#include "Texture.h"


Texture::Texture(){
    path = "";
    used = false;
}

Texture::Texture(const char* path){
    loadFromFile(path);
    used = true;
}

Texture::Texture(const Texture& v){
    this->path = v.path;
    this->used = v.used;
}

Texture& Texture::operator = (const Texture& v){
    this->path = v.path;
    this->used = v.used;

    return *this;
}

Texture::~Texture(){

}

void Texture::loadFromFile(const char* path){
    this->path = path;
    used = true;
}

const char* Texture::getFilePath(){
    return path.c_str();
}

bool Texture::isUsed(){
    return used;
}
