//
// (c) 2024 Eduardo Doria.
//

#include "CameraRender.h"

#include "sokol/SokolCamera.h"

using namespace Supernova;

CameraRender::CameraRender(){ }

CameraRender::CameraRender(const CameraRender& rhs) : backend(rhs.backend) { }

CameraRender& CameraRender::operator=(const CameraRender& rhs) { 
    backend = rhs.backend; 
    return *this; 
}

CameraRender::~CameraRender(){ }

void CameraRender::setClearColor(Vector4 clearColor){
    backend.setClearColor(clearColor);
}

void CameraRender::startFrameBuffer(FramebufferRender* framebuffer, size_t face){
    backend.startFrameBuffer(framebuffer, face);
}

void CameraRender::startFrameBuffer(int width, int height){
    backend.startFrameBuffer(width, height);
}

void CameraRender::startFrameBuffer(){
    backend.startFrameBuffer();
}

void CameraRender::applyViewport(Rect rect){
    backend.applyViewport(rect);
}

void CameraRender::applyScissor(Rect rect){
    backend.applyScissor(rect);
}

void CameraRender::endFrameBuffer(){
    backend.endFrameBuffer();
}