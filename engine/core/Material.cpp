#include "Material.h"


using namespace Supernova;

Material::Material(){
    this->textureType = S_TEXTURE_2D;
    transparent = false;
    textureRect = NULL;
    color = Vector4(0.0, 0.0, 0.0, 0.0);
}

Material::~Material(){
    if (textureRect)
        delete textureRect;
}

Material::Material(const Material& s){
    this->textures = s.textures;
    this->color = s.color;
    this->textureType = s.textureType;
}

Material& Material::operator = (const Material& s){
    this->textures = s.textures;
    this->color = s.color;
    this->textureType = s.textureType;
    
    return *this;
}

void Material::setTexture(std::string texture){
    if (textures.size() == 0){
        textures.push_back(texture);
    }else{
        textures[0] = texture;
    }
}

void Material::setColor(Vector4 color){
    
    if (color.w != 1){
        transparent = true;
    }
    
    this->color = color;
}

void Material::setTextureType(int textureType){
    this->textureType = textureType;
}

void Material::setTextureCube(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down){
    this->textureType = S_TEXTURE_CUBE;
    textures.clear();
    textures.push_back(right);
    textures.push_back(left);
    textures.push_back(up);
    textures.push_back(down);
    textures.push_back(back);
    textures.push_back(front);
}

void Material::setTextureRect(float x, float y, float width, float height){
    if (textureRect)
        textureRect->setRect(x, y, width, height);
    else
        textureRect = new Rect(x, y, width, height);
}

std::vector<std::string> Material::getTextures(){
    return textures;
}

Vector4* Material::getColor(){
    return &color;
}

int Material::getTextureType(){
    return textureType;
}

Rect* Material::getTextureRect(){
    return textureRect;
}
