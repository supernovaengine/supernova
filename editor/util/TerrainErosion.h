// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include "util/TerrainMapUtils.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace doriax::editor{

    // Erosion over a normalized height buffer. The brush passes its own weight through
    // "falloff", which is what keeps the effect inside the stamp.
    class TerrainErosion{

    public:

        // Weight for a texel, 0 outside the brush
        using FalloffFunc = std::function<float(int, int)>;

        // Droplets carry sediment downhill and drop it where the slope eases. heightScale
        // converts a height difference into texel steps, so any map resolution erodes alike.
        static void hydraulic(std::vector<float>& heights, int width, int height, const TerrainMapRegion& region,
                              float heightScale, int droplets, uint32_t seed, const FalloffFunc& falloff);

        // Slopes steeper than talus shed onto their lower neighbours: the angle of repose
        static void thermal(std::vector<float>& heights, int width, int height, const TerrainMapRegion& region,
                            float heightScale, float talus, float rate, const FalloffFunc& falloff);
    };

}
