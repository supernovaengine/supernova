#include "Texture.h"

using namespace Supernova;

Texture::Texture(){
    alphaChannel = false;
    width = 0;
    height = 0;
}

Texture::Texture(std::shared_ptr<TextureRender> textureRender, bool alphaChannel, int width, int height){
    this->textureRender = textureRender;
    this->alphaChannel = alphaChannel;
    this->width = width;
    this->height = height;
}

Texture::~Texture(){
    
}

void Texture::setTextureRender(std::shared_ptr<TextureRender> textureRender){
    this->textureRender = textureRender;
}

void Texture::setAlphaChannel(bool alphaChannel){
    this->alphaChannel = alphaChannel;
}

void Texture::setSize(int width, int height){
    this->width = width;
    this->height = height;
}

std::shared_ptr<TextureRender> Texture::getTextureRender(){
    return textureRender;
}

bool Texture::hasAlphaChannel(){
    return alphaChannel;
}

int Texture::getWidth(){
    return width;
}

int Texture::getHeight(){
    return height;
}
