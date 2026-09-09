// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef KEYFRAMETRACKS_COMPONENT_H
#define KEYFRAMETRACKS_COMPONENT_H

#include "Engine.h"
#include "action/Ease.h"

namespace doriax{

    struct DORIAX_API KeyframeTracksComponent{
        std::vector<float> times;
        // Per-segment easing: easings[i] shapes the interpolation from key i to
        // key i+1. Empty or shorter than the segment count means linear there.
        // GLTF LINEAR clips leave this empty; GLTF STEP clips fill it with
        // EaseType::STEP. On tracks with Hermite tangents (GLTF CUBICSPLINE)
        // the eased factor feeds the cubic curve, so leave this empty for
        // spec-faithful playback.
        std::vector<EaseType> easings;
        int index = 0;
        float interpolation = 0;
    };

}

#endif //KEYFRAMETRACKS_COMPONENT_H