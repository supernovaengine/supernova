#include "Texture.h"

#include "render/TextureManager.h"

using namespace Supernova;

Texture::Texture(){
    colorFormat = false;
    width = 0;
    height = 0;
}

Texture::Texture(std::shared_ptr<TextureRender> textureRender, int colorFormat, int width, int height){
    this->textureRender = textureRender;
    this->colorFormat = colorFormat;
    this->width = width;
    this->height = height;
}

Texture::~Texture(){
    textureRender.reset();
}

Texture::Texture(const Texture& t){
    this->textureRender = t.textureRender;
    this->colorFormat = t.colorFormat;
    this->width = t.width;
    this->height = t.height;
}

Texture& Texture::operator = (const Texture& t){
    this->textureRender = t.textureRender;
    this->colorFormat = t.colorFormat;
    this->width = t.width;
    this->height = t.height;

    return *this;
}

void Texture::setTextureRender(std::shared_ptr<TextureRender> textureRender){
    this->textureRender = textureRender;
}

void Texture::setColorFormat(int colorFormat){
    this->colorFormat = colorFormat;
}

void Texture::setSize(int width, int height){
    this->width = width;
    this->height = height;
}

std::shared_ptr<TextureRender> Texture::getTextureRender(){
    return textureRender;
}

int Texture::getColorFormat(){
    return colorFormat;
}

bool Texture::hasAlphaChannel(){

    if (colorFormat == S_COLOR_GRAY_ALPHA ||
        colorFormat == S_COLOR_RGB_ALPHA ||
        colorFormat == S_COLOR_ALPHA)
        return true;

    return false;
}

int Texture::getWidth(){
    return width;
}

int Texture::getHeight(){
    return height;
}

void Texture::destroy(){
    textureRender.reset();
    TextureManager::deleteUnused();
}
