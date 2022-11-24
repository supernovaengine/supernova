#include "FramebufferRender.h"

using namespace Supernova;

FramebufferRender::FramebufferRender(){ }

FramebufferRender::FramebufferRender(const FramebufferRender& rhs) : backend(rhs.backend) { }

FramebufferRender& FramebufferRender::operator=(const FramebufferRender& rhs) { 
    backend = rhs.backend;
    return *this; 
}

FramebufferRender::~FramebufferRender(){
    //Cannot destroy because its a handle
}

bool FramebufferRender::createFramebuffer(TextureType textureType, int width, int height, TextureFilter minFilter, TextureFilter magFilter, bool shadowMap){
    return backend.createFramebuffer(textureType, width, height, minFilter, magFilter, shadowMap);
}

void FramebufferRender::destroyFramebuffer(){
    backend.destroyFramebuffer();
}

bool FramebufferRender::isCreated(){
    return backend.isCreated();
}

TextureRender& FramebufferRender::getColorTexture(){
    return backend.getColorTexture();
}