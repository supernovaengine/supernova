#include "SokolScene.h"

#include "System.h"

#include "sokol_gfx.h"

using namespace Supernova;

SokolScene::SokolScene(){
    pass_action = {0};
    pass_action.colors[0] = { .action = SG_ACTION_CLEAR, .value = { 0.1f, 0.1f, 0.1f, 1.0f } };
}

SokolScene::SokolScene(const SokolScene& rhs) : pass_action(rhs.pass_action) {}

SokolScene& SokolScene::operator=(const SokolScene& rhs) { 
    pass_action = rhs.pass_action; 
    return *this;
}

SokolScene::~SokolScene(){

}

void SokolScene::setClearColor(Vector4 clearColor){
    pass_action.colors[0].value = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
}

void SokolScene::startFrameBuffer(FramebufferRender* framebuffer){
    sg_pass pass = framebuffer->backend.get();
    sg_begin_pass(pass, &pass_action);
}

void SokolScene::startDefaultFrameBuffer(int width, int height){
    sg_begin_default_pass(&pass_action, width, height);
}

void SokolScene::applyViewport(Rect rect){
    sg_apply_viewport(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), false);
}

void SokolScene::endFrameBuffer(){
    sg_end_pass();
}
