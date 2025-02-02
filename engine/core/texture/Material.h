//
// (c) 2024 Eduardo Doria.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "math/Vector3.h"
#include "texture/Texture.h"

namespace Supernova{

    struct SUPERNOVA_API Material{
        // --- start shader part
        Vector4 baseColorFactor = Vector4(1.0, 1.0, 1.0, 1.0);  // linear color
        float metallicFactor = 1.0;
        float roughnessFactor = 1.0;
        uint8_t _pad_24[8];
        Vector3 emissiveFactor = Vector3(0.0, 0.0, 0.0);  // linear color
        uint8_t _pad_44[4];
        Vector3 ambientLight = Vector3(1.0, 1.0, 1.0);  // linear color
        float ambientIntensity = 0.1;
        // --- end shader part

        Texture baseColorTexture;
        Texture emissiveTexture;
        Texture metallicRoughnessTexture;
        Texture occlusionTexture;
        Texture normalTexture;

        // material name or file
        std::string name = "";
    };

}

#endif //MATERIAL_H