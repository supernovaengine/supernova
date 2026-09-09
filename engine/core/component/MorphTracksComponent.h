// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MORPHTRACKS_COMPONENT_H
#define MORPHTRACKS_COMPONENT_H

#include "Engine.h"

namespace doriax{

    struct DORIAX_API MorphTracksComponent{
        std::vector<std::vector<float>> values;
        // Per-key Hermite tangents (GLTF CUBICSPLINE); empty on linear tracks.
        // Cubic interpolation runs only while both stay sized to values — any
        // code editing values must keep them mirrored (the runtime falls back
        // to linear on mismatch).
        std::vector<std::vector<float>> inTangents;
        std::vector<std::vector<float>> outTangents;
    };

}

#endif //MORPHTRACKS_COMPONENT_H