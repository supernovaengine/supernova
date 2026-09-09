// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SokolShader_h
#define SokolShader_h

#include "render/Render.h"
#include "shader/ShaderData.h"
#include "sokol_gfx.h"

namespace doriax{

    // textures and storage buffers share the sg_bindings.views[] bindspace, but
    // shader reflection (ShaderData) keeps independent slot counters for each type,
    // so storage-buffer view slots are placed after the texture range
    enum { SOKOL_STORAGEBUFFER_VIEW_SLOT_OFFSET = SG_MAX_VIEW_BINDSLOTS - 8 };

    class SokolShader{

    private:
        sg_shader shader;

        int roundup(int val, int round_to);
        sg_uniform_type uniformToSokolType(ShaderUniformType type);
        sg_uniform_type flattenedUniformToSokolType(ShaderUniformType type);
        sg_image_sample_type textureSamplerToSokolType(TextureSamplerType type);
        sg_image_type textureToSokolType(TextureType type);
        sg_sampler_type samplerToSokolType(SamplerType type);

    public:
        SokolShader();
        SokolShader(const SokolShader& rhs);
        SokolShader& operator=(const SokolShader& rhs);

        bool createShader(ShaderData& shaderData);
        void destroyShader();
        bool isCreated();
        bool isFailed();

        sg_shader get();
    };
}


#endif /* SokolShader_h */
