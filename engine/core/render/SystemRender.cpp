//
// (c) 2023 Eduardo Doria.
//

#include "SystemRender.h"

#include "sokol/SokolSystem.h"

using namespace Supernova;

void SystemRender::setup(){
    SokolSystem::setup();
}

void SystemRender::executeQueue(){
    SokolSystem::executeQueue();
}

void SystemRender::commit(){
    SokolSystem::commit();
}

void SystemRender::shutdown(){
    SokolSystem::shutdown();
}

void SystemRender::scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer){
    SokolSystem::scheduleCleanup(cleanupFunc, cleanupData, numFramesToDefer);
}

void SystemRender::addQueueCommand(void (*custom_cb)(void* custom_data), void* custom_data){
    SokolSystem::addQueueCommand(custom_cb, custom_data);
}