// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SHADERBUILDTYPES_H
#define SHADERBUILDTYPES_H

#include <cstdint>
#include "Engine.h"
#include "ShaderData.h"

namespace doriax{

    typedef uint64_t ShaderKey;

    // What a shader is compiled for. One value per shader language string, which is
    // what names the file the runtime loads. Metal is split because the cross-compiler
    // emits different MSL for macOS and iOS.
    enum class ShaderBackend{
        GLCore,      // glsl410
        GLES3,       // glsl300es
        D3D11,       // hlsl5
        MetalMacOS,  // msl21macos
        MetalIOS,    // msl21ios
        Vulkan       // spirv10
    };

    struct ShaderBuildResult {
        ShaderData data;
        ResourceLoadState state;

        ShaderBuildResult() : state(ResourceLoadState::NotStarted) {}
        ShaderBuildResult(const ShaderData& shaderData, ResourceLoadState buildState)
            : data(shaderData), state(buildState) {}

        explicit operator bool() const {
            return state == ResourceLoadState::Finished;
        }
    };

}

#endif // SHADERBUILDTYPES_H