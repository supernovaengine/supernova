#include "TextureRender.h"

using namespace Supernova;

TextureRender::TextureRender(){
 }

TextureRender::TextureRender(const TextureRender& rhs) : backend(rhs.backend) { }

TextureRender& TextureRender::operator=(const TextureRender& rhs) { 
    backend = rhs.backend;
    return *this; 
}

TextureRender::~TextureRender(){
    //Cannot destroy because its a handle
}

bool TextureRender::createTexture(std::string label, int width, int height, ColorFormat colorFormat, TextureType type, int numFaces, TextureDataSize* texData){
    return backend.createTexture(label, width, height, colorFormat, type, numFaces, texData);
}

bool TextureRender::createFramebufferTexture(TextureType type, bool depth, bool shadowMap, int width, int height){
    return backend.createFramebufferTexture(type, depth, shadowMap, width, height);
}

void TextureRender::destroyTexture(){
    backend.destroyTexture();
}