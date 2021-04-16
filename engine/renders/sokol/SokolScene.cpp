#include "SokolScene.h"

#include "System.h"

#include "sokol_gfx.h"

using namespace Supernova;

SokolScene::SokolScene(){
    /* default pass action (clear to grey) */
    pass_action = {0};
    pass_action.colors[0] = { .action = SG_ACTION_CLEAR, .value = { 0.0f, 0.0f, 0.0f, 1.0f } };
}

SokolScene::SokolScene(const SokolScene& rhs) : pass_action(rhs.pass_action) {}

SokolScene& SokolScene::operator=(const SokolScene& rhs) { 
    pass_action = rhs.pass_action; 
    return *this;
}

SokolScene::~SokolScene(){

}

void SokolScene::startFrameBuffer(){
    sg_begin_default_pass(&pass_action, System::instance().getScreenWidth(), System::instance().getScreenHeight());
}

void SokolScene::applyViewport(Rect rect){
    sg_apply_viewport(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), false);
}

void SokolScene::endFrameBuffer(){
    sg_end_pass();
    sg_commit();
}
