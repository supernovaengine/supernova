#include "Material.h"

#include "image/TextureLoader.h"

using namespace Supernova;

Material::Material(){
    texture = NULL;
    textureRect = NULL;
    color = Vector4(1.0, 1.0, 1.0, 1.0);
    textureOwned = true;
}

Material::~Material(){
    if (texture && textureOwned)
        delete texture;
    if (textureRect)
        delete textureRect;
}

Material::Material(const Material& s){
    this->texture = s.texture;
    this->color = s.color;
    this->textureRect = s.textureRect;
    this->textureOwned = s.textureOwned;
}

Material& Material::operator = (const Material& s){
    this->texture = s.texture;
    this->color = s.color;
    this->textureRect = s.textureRect;
    this->textureOwned = s.textureOwned;
    
    return *this;
}

void Material::setTexture(Texture* texture){
    if (this->texture && textureOwned)
        delete this->texture;
    
    this->texture = texture;
    this->textureOwned = false;
}

void Material::setTexturePath(std::string texture_path){
    if (this->texture && textureOwned)
        delete this->texture;
    
    this->texture = new Texture(texture_path);
    this->textureOwned = true;
    this->texture->setDataOwned(true);
}

void Material::setColor(Vector4 color){
    this->color = color;
}

void Material::setTextureCube(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down){
    
    std::vector<std::string> textures;
    textures.push_back(right);
    textures.push_back(left);
    textures.push_back(up);
    textures.push_back(down);
    textures.push_back(back);
    textures.push_back(front);
    
    TextureLoader image;
    std::vector<TextureData*> texturesData;
    
    std::string id = "cube|";
    for (int i = 0; i < textures.size(); i++){
        texturesData.push_back(image.loadTextureData(textures[i].c_str()));
        texturesData.back()->resamplePowerOfTwo();
        id = id + "|" + textures[i];
    }
    
    if (this->texture && textureOwned)
        delete this->texture;
    
    this->texture = new Texture(texturesData, id);
    this->texture->setType(S_TEXTURE_CUBE);
    this->texture->setDataOwned(true);
}

void Material::setTextureRect(float x, float y, float width, float height){
    if (textureRect)
        textureRect->setRect(x, y, width, height);
    else
        textureRect = new Rect(x, y, width, height);
}

std::string Material::getTexturePath(){
    if (texture)
        return texture->getId();
    
    return "";
}

Texture* Material::getTexture(){
    return texture;
}

Vector4* Material::getColor(){
    return &color;
}

Rect* Material::getTextureRect(){
    return textureRect;
}

bool Material::isTransparent(){
    if (texture && texture->getType() == S_TEXTURE_2D && texture->hasAlphaChannel()) {
        return true;
    }
    if (color.w != 1){
        return true;
    }
    return false;
}
