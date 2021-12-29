#include "SokolSystem.h"

#include "System.h"
#include "sokol_gfx.h"

using namespace Supernova;

void SokolSystem::setup(){
    /* setup sokol_gfx */
    sg_desc desc = {0};
    //desc.pipeline_pool_size = 2048;
    //desc.buffer_pool_size = 1024;
    desc.context = System::instance().getSokolContext();

    sg_setup(&desc);
}

void SokolSystem::commit(){
    sg_commit();
}

void SokolSystem::shutdown(){
    sg_shutdown();
}