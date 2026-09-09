// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ShaderRender.h"

#include "SystemRender.h"
#include "Engine.h"

using namespace doriax;

ShaderRender::ShaderRender(){ }

ShaderRender::ShaderRender(const ShaderRender& rhs) : backend(rhs.backend), shaderData(rhs.shaderData), loading(rhs.loading) { }

ShaderRender& ShaderRender::operator=(const ShaderRender& rhs) {
    backend = rhs.backend;
    shaderData = rhs.shaderData;
    loading = rhs.loading;
    return *this;
}

ShaderRender::~ShaderRender(){
    //Cannot destroy because its a handle
}

bool ShaderRender::createShader(ShaderData& shaderData){
    // Skip while a deferred create is in flight: re-assigning shaderData would
    // free the source the queued sg_shader_desc points into and corrupt the build.
    if (Engine::isViewLoaded() && !isCreated() && !loading) {
        this->shaderData = shaderData;

        loading = true;

        bool ret = backend.createShader(this->shaderData);

        // Via the command stream so cleanup runs after MAKE_SHADER (a frame-counted
        // scheduleCleanup could fire before the build -> empty source on async path).
        SystemRender::addQueueCommand(ShaderRender::cleanupShader, this);

        return ret;
    }else{
        return false;
    }
}

void ShaderRender::destroyShader(){
    backend.destroyShader();
}

bool ShaderRender::isCreated(){
    return backend.isCreated();
}

bool ShaderRender::isFailed(){
    return backend.isFailed();
}

void ShaderRender::cleanupShader(void* data){
    ShaderRender* self = static_cast<ShaderRender*>(data);
    self->shaderData.releaseSourceData(); //Keep only reflection info
    self->loading = false; //Build executed; allow retry on failure
}
