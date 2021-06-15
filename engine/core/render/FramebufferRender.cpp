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

bool FramebufferRender::createFramebuffer(int width, int height){
    return backend.createFramebuffer(width, height);
}

void FramebufferRender::destroyFramebuffer(){
    backend.destroyFramebuffer();
}

TextureRender* FramebufferRender::getColorTexture(){
    return backend.getColorTexture();
}