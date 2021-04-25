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

void SceneRender::startFrameBuffer(FramebufferRender* framebuffer){
    backend.startFrameBuffer(framebuffer);
}

void SceneRender::startDefaultFrameBuffer(int width, int height){
    backend.startDefaultFrameBuffer(width, height);
}

void SceneRender::applyViewport(Rect rect){
    backend.applyViewport(rect);
}

void SceneRender::endFrameBuffer(){
    backend.endFrameBuffer();
}