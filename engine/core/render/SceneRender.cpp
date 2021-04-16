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

void SceneRender::startFrameBuffer(){
    backend.startFrameBuffer();
}

void SceneRender::applyViewport(Rect rect){
    backend.applyViewport(rect);
}

void SceneRender::endFrameBuffer(){
    backend.endFrameBuffer();
}