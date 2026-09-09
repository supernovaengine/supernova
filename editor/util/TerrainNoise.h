// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>
#include <cstdint>

namespace doriax::editor{

    // Value noise on an integer lattice, shared by the noise brush and the AI generators.
    class TerrainNoise{

    public:

        inline static float hash(int x, int y, uint32_t seed) {
            uint32_t h = seed ^ 0x9E3779B9u;
            h ^= static_cast<uint32_t>(x) + 0x85EBCA6Bu + (h << 6) + (h >> 2);
            h ^= static_cast<uint32_t>(y) + 0xC2B2AE35u + (h << 6) + (h >> 2);
            h ^= h >> 16;
            h *= 0x7FEB352Du;
            h ^= h >> 15;
            h *= 0x846CA68Bu;
            h ^= h >> 16;
            return static_cast<float>(h & 0x00FFFFFFu) / static_cast<float>(0x00FFFFFFu);
        }

        inline static float value(float x, float y, uint32_t seed) {
            const int x0 = static_cast<int>(std::floor(x));
            const int y0 = static_cast<int>(std::floor(y));
            const float tx = fade(x - static_cast<float>(x0));
            const float ty = fade(y - static_cast<float>(y0));

            const float a = hash(x0, y0, seed);
            const float b = hash(x0 + 1, y0, seed);
            const float c = hash(x0, y0 + 1, seed);
            const float d = hash(x0 + 1, y0 + 1, seed);

            const float lower = a + (b - a) * tx;
            const float upper = c + (d - c) * tx;
            return lower + (upper - lower) * ty;
        }

        // Octaves at doubling frequency and halving weight, normalized to [-1, 1]
        inline static float fractal(float x, float y, uint32_t seed, int octaves) {
            float sum = 0.0f;
            float weight = 1.0f;
            float weightSum = 0.0f;
            float frequency = 1.0f;

            for (int o = 0; o < octaves; o++) {
                sum += (value(x * frequency, y * frequency, seed + static_cast<uint32_t>(o * 1013)) * 2.0f - 1.0f) * weight;
                weightSum += weight;
                weight *= 0.5f;
                frequency *= 2.0f;
            }

            return weightSum > 0.0f ? sum / weightSum : 0.0f;
        }

    private:

        inline static float fade(float t) {
            return t * t * (3.0f - 2.0f * t);
        }
    };

}
