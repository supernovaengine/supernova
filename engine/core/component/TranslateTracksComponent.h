// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TRANSLATETRACKS_COMPONENT_H
#define TRANSLATETRACKS_COMPONENT_H

#include "Engine.h"

namespace doriax{

    struct DORIAX_API TranslateTracksComponent{
        std::vector<Vector3> values;
        // Per-key Hermite tangents (GLTF CUBICSPLINE); empty on linear tracks.
        // Cubic interpolation runs only while both stay sized to values — any
        // code editing values must keep them mirrored (the runtime falls back
        // to linear on mismatch).
        std::vector<Vector3> inTangents;
        std::vector<Vector3> outTangents;
    };

}

#endif //TRANSLATETRACKS_COMPONENT_H