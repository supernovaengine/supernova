//
// (c) 2023 Eduardo Doria.
//

#include "SokolSystem.h"

#include "System.h"
#include "sokol_gfx.h"
#include "SokolCmdBuffer.h"

// Render thread
// 
// execute_commands() in render loop
// wait_for_flush() on termination
//
//
// Update thread
// 
// add_command_xxx()
// commit_commands() when you're done for the frame
// flush_commands() on termination, before exiting the thread

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
    SokolCmdBuffer::commit_commands();
    SokolCmdBuffer::execute_commands();
    sg_commit();
}

void SokolSystem::shutdown(){
    SokolCmdBuffer::flush_commands();
    SokolCmdBuffer::wait_for_flush();
    SokolCmdBuffer::finish();
    sg_shutdown();
}

void SokolSystem::scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer){
    SokolCmdBuffer::schedule_cleanup(cleanupFunc, cleanupData, numFramesToDefer);
}