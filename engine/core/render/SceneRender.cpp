//
// (c) 2024 Eduardo Doria.
//

#include "SceneRender.h"

#include "sokol/SokolScene.h"

using namespace Supernova;

SceneRender::SceneRender(){ }

SceneRender::SceneRender(const SceneRender& rhs) : backend(rhs.backend) { }

SceneRender& SceneRender::operator=(const SceneRender& rhs) { 
    backend = rhs.backend; 
    return *this; 
}

SceneRender::~SceneRender(){ }

void SceneRender::setClearColor(Vector4 clearColor){
    backend.setClearColor(clearColor);
}

void SceneRender::startFrameBuffer(FramebufferRender* framebuffer, size_t face){
    backend.startFrameBuffer(framebuffer, face);
}

void SceneRender::startFrameBuffer(){
    backend.startFrameBuffer();
}

void SceneRender::applyViewport(Rect rect){
    backend.applyViewport(rect);
}

void SceneRender::applyScissor(Rect rect){
    backend.applyScissor(rect);
}

void SceneRender::endFrameBuffer(){
    backend.endFrameBuffer();
}