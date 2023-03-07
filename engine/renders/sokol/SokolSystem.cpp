//
// (c) 2023 Eduardo Doria.
//

#include "SokolSystem.h"

#include "System.h"
#include "sokol_gfx.h"
#include "SokolCmdQueue.h"

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

    SokolCmdQueue::start();
}

void SokolSystem::executeQueue(){
    SokolCmdQueue::commit_commands();
    SokolCmdQueue::execute_commands();
}

void SokolSystem::commit(){
    sg_commit();
}

void SokolSystem::shutdown(){
    SokolCmdQueue::flush_commands();
    SokolCmdQueue::wait_for_flush();
    SokolCmdQueue::finish();
    sg_shutdown();
}

void SokolSystem::scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer){
    SokolCmdQueue::schedule_cleanup(cleanupFunc, cleanupData, numFramesToDefer);
}