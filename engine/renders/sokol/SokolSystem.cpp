//
// (c) 2023 Eduardo Doria.
//

#include "SokolSystem.h"

#include "System.h"
#include "sokol_gfx.h"
#include "SokolCmdBuffer.h"

using namespace Supernova;

void SokolSystem::setup(){
    /* setup sokol_gfx */
    sg_desc desc = {0};
    //desc.pipeline_pool_size = 2048;
    //desc.buffer_pool_size = 1024;
    desc.context = System::instance().getSokolContext();

    sg_setup(&desc);

    SokolCmdBuffer::start();
}

void SokolSystem::commit(){
    sg_commit();
}

void SokolSystem::shutdown(){
    SokolCmdBuffer::finish();

    sg_shutdown();
}

void SokolSystem::commitCommands(){
    SokolCmdBuffer::commit_commands();
}

void SokolSystem::executeCommands(){
    SokolCmdBuffer::execute_commands();
}

void SokolSystem::flushCommands(){
    SokolCmdBuffer::flush_commands();
}

void SokolSystem::waitForFlush(){
    SokolCmdBuffer::wait_for_flush();
}

void SokolSystem::scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer){
    SokolCmdBuffer::schedule_cleanup(cleanupFunc, cleanupData, numFramesToDefer);
}