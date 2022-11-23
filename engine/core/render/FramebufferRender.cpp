#include "FramebufferRender.h"

using namespace Supernova;

FramebufferRender::FramebufferRender(){
    this->width = 0;
    this->height = 0;
    this->version = 0;
 }

FramebufferRender::FramebufferRender(const FramebufferRender& rhs) : backend(rhs.backend) { }

FramebufferRender& FramebufferRender::operator=(const FramebufferRender& rhs) { 
    backend = rhs.backend; 
    width = rhs.width;
    height = rhs.height;
    version = rhs.version;
    return *this; 
}

FramebufferRender::~FramebufferRender(){
    //Cannot destroy because its a handle
}

bool FramebufferRender::createFramebuffer(TextureType textureType, int width, int height, bool shadowMap){
    this->width = width;
    this->height = height;
    this->version++;
    return backend.createFramebuffer(textureType, width, height, shadowMap);
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

int FramebufferRender::getWidth(){
    return this->width;
}

int FramebufferRender::getHeight(){
    return this->height;
}