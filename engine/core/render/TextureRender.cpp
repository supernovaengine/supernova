//
// (c) 2024 Eduardo Doria.
//

#include "TextureRender.h"
#include "Engine.h"

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

bool TextureRender::createTexture(
                std::string label, int width, int height, 
                ColorFormat colorFormat, TextureType type, int numFaces, void* data[6], size_t size[6], 
                TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV){
    if (Engine::isViewLoaded())
        return backend.createTexture(label, width, height, colorFormat, type, numFaces, data, size, minFilter, magFilter, wrapU, wrapV);
    else
        return false;
}

bool TextureRender::createFramebufferTexture(
                TextureType type, bool depth, bool shadowMap, int width, int height, 
                TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV){
    if (Engine::isViewLoaded())
        return backend.createFramebufferTexture(type, depth, shadowMap, width, height, minFilter, magFilter, wrapU, wrapV);
    else
        return false;
}

void TextureRender::destroyTexture(){
    backend.destroyTexture();
}

uint32_t TextureRender::getGLHandler(){
    return backend.getGLHandler();
}