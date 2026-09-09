// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SCALETRACKS_COMPONENT_H
#define SCALETRACKS_COMPONENT_H

#include "Engine.h"

namespace doriax{

    struct DORIAX_API ScaleTracksComponent{
        std::vector<Vector3> values;
        // Per-key Hermite tangents (GLTF CUBICSPLINE); empty on linear tracks.
        // Cubic interpolation runs only while both stay sized to values — any
        // code editing values must keep them mirrored (the runtime falls back
        // to linear on mismatch).
        std::vector<Vector3> inTangents;
        std::vector<Vector3> outTangents;
    };

}

#endif //SCALETRACKS_COMPONENT_H