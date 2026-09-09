// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef OCCLUDER2D_COMPONENT_H
#define OCCLUDER2D_COMPONENT_H

#include "math/Vector2.h"
#include "Engine.h"
#include <vector>

namespace doriax{

    enum class Occluder2DShape{
        AUTO_QUAD, // segments derived from the sibling mesh's local AABB (xy)
        POLYGON    // explicit local-space points
    };

    struct DORIAX_API Occluder2DComponent{
        Occluder2DShape shape = Occluder2DShape::AUTO_QUAD;

        std::vector<Vector2> points; // local-space vertices, used when shape == POLYGON
        bool closed = true;          // closed loop (last connects to first) vs open chain
        bool enabled = true;
    };

}

#endif //OCCLUDER2D_COMPONENT_H
