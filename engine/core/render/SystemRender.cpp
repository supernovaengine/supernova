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