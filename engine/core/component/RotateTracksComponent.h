// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ROTATETRACKS_COMPONENT_H
#define ROTATETRACKS_COMPONENT_H

#include "Engine.h"

namespace doriax{

    struct DORIAX_API RotateTracksComponent{
        std::vector<Quaternion> values;
        // Per-key Hermite tangents (GLTF CUBICSPLINE); empty on linear tracks.
        // Cubic interpolation runs only while both stay sized to values — any
        // code editing values must keep them mirrored (the runtime falls back
        // to linear on mismatch).
        std::vector<Quaternion> inTangents;
        std::vector<Quaternion> outTangents;
    };

}

#endif //ROTATETRACKS_COMPONENT_H