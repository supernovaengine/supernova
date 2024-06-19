//
// (c) 2024 Eduardo Doria.
//

#include "SokolScene.h"

#include "System.h"
#include "SokolCmdQueue.h"

#include "sokol_gfx.h"

using namespace Supernova;

SokolScene::SokolScene(){
    //pass_action = {0};
    pass_action.colors[0].load_action = SG_LOADACTION_LOAD;
}

SokolScene::SokolScene(const SokolScene& rhs) : pass_action(rhs.pass_action) {}

SokolScene& SokolScene::operator=(const SokolScene& rhs) { 
    pass_action = rhs.pass_action; 
    return *this;
}

SokolScene::~SokolScene(){

}

void SokolScene::setClearColor(Vector4 clearColor){
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].clear_value = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
}

void SokolScene::startFrameBuffer(FramebufferRender* framebuffer, size_t face){
    sg_pass pass = {0};
    pass.action = pass_action;
    pass.attachments = framebuffer->backend.get(face);
    //SokolCmdQueue::add_command_begin_pass(pass);
    sg_begin_pass(pass);
}

void SokolScene::startFrameBuffer(){
    sg_pass pass = {0};
    pass.action = pass_action;
    pass.swapchain = System::instance().getSokolSwapchain();
    //SokolCmdQueue::add_command_begin_pass(pass);
    sg_begin_pass(pass);
}

void SokolScene::applyViewport(Rect rect){
    //SokolCmdQueue::add_command_apply_viewport((int)rect.getX(), (int)rect.getY(), (int)rect.getWidth(), (int)rect.getHeight(), false);
    sg_apply_viewport((int)rect.getX(), (int)rect.getY(), (int)rect.getWidth(), (int)rect.getHeight(), false);
}

void SokolScene::applyScissor(Rect rect){
    //SokolCmdQueue::add_command_apply_scissor_rect((int)rect.getX(), (int)rect.getY(), (int)rect.getWidth(), (int)rect.getHeight(), false);
    sg_apply_scissor_rect((int)rect.getX(), (int)rect.getY(), (int)rect.getWidth(), (int)rect.getHeight(), false);
}

void SokolScene::endFrameBuffer(){
    //SokolCmdQueue::add_command_end_pass();
    sg_end_pass();
}
