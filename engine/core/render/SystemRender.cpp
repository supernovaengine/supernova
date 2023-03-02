//
// (c) 2023 Eduardo Doria.
//

#include "SystemRender.h"

#include "sokol/SokolSystem.h"

using namespace Supernova;

void SystemRender::setup(){
    SokolSystem::setup();
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