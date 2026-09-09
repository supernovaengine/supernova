// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef LIGHT2D_COMPONENT_H
#define LIGHT2D_COMPONENT_H

#include "math/Vector3.h"
#include "Engine.h"

namespace doriax{

    struct DORIAX_API Light2DComponent{
        Vector3 color = Vector3(1.0, 1.0, 1.0);

        float intensity = 1.0;
        float range = 200.0; // radius in world units
        float falloff = 1.0; // attenuation exponent: pow(1 - d/range, falloff)
        float height = 0.0;  // virtual Z of the light for normal-map response (0 = pure radial, ignores normals)

        bool shadows = false;
        float shadowBias = 0.01f;    // in normalized-distance units (dist/range)
        float shadowSoftness = 2.0f; // PCF spread in 1D-map texels (0 = hard)
        unsigned int mapResolution = 512; // width of this light's 1D polar row; atlas uses max across lights

        // row in the 1D polar shadow atlas; -1 when the light has no shadow allocation this frame
        int shadowMapIndex = -1;
    };

}

#endif //LIGHT2D_COMPONENT_H
