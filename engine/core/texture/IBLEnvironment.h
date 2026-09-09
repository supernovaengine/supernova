// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef IBLENVIRONMENT_H
#define IBLENVIRONMENT_H

#include "texture/TextureData.h"
#include "render/TextureRender.h"
#include <array>
#include <string>

namespace doriax{

    // Generates image based lighting (IBL) environment maps on CPU from a
    // LDR cubemap, following the maps used by glTF-Sample-Renderer:
    //  - lambertian (irradiance) cubemap, cosine convolved and divided by PI
    //  - GGX prefiltered cubemap, roughness mapped to mip levels
    class DORIAX_API IBLEnvironment{
    public:
        static const int SPECULAR_BASE_SIZE = 128;
        static const int SPECULAR_MIPS = 8;     // 128,64,32,16,8,4,2,1 (shader ENV_GGX_LODS = SPECULAR_MIPS - 1)
        static const int IRRADIANCE_SIZE = 16;

        // faces follow the cubemap convention: 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
        static bool generate(const std::string& label, std::array<TextureData,6>& faces, TextureRender& irradianceMap, TextureRender& prefilteredMap);
    };

}

#endif //IBLENVIRONMENT_H
