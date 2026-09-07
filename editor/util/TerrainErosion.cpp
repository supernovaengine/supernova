// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#include "TerrainErosion.h"

#include "TerrainNoise.h"

#include <algorithm>
#include <cmath>

using namespace doriax;

static constexpr int DROPLET_STEPS = 32;
static constexpr float DROPLET_INERTIA = 0.05f;
static constexpr float DROPLET_CAPACITY = 4.0f;
static constexpr float DROPLET_MIN_SLOPE = 0.01f;
static constexpr float DROPLET_ERODE_RATE = 0.3f;
static constexpr float DROPLET_DEPOSIT_RATE = 0.3f;
static constexpr float DROPLET_EVAPORATION = 0.05f;
static constexpr float DROPLET_GRAVITY = 4.0f;
// Cutting a single texel leaves pits, so the cut is spread over its neighbours
static constexpr int ERODE_RADIUS = 1;
static constexpr int ERODE_TAPS = (ERODE_RADIUS * 2 + 1) * (ERODE_RADIUS * 2 + 1);

namespace {

    struct HeightField{
        std::vector<float>& heights;
        int width;
        int height;
        editor::TerrainMapRegion region;

        float at(int x, int y) const{
            x = std::clamp(x, 0, width - 1);
            y = std::clamp(y, 0, height - 1);
            return heights[static_cast<size_t>(y) * width + x];
        }

        // Writes stay inside the stamp: outside it nothing would reach the map, and the
        // stroke's undo patch would not cover it. Returns what landed, so a droplet is
        // only credited for material it actually moved.
        float add(int x, int y, float amount){
            if (x < region.minX || y < region.minY || x > region.maxX || y > region.maxY){
                return 0.0f;
            }
            float& value = heights[static_cast<size_t>(y) * width + x];
            const float previous = value;
            value = std::clamp(value + amount, 0.0f, 1.0f);
            return value - previous;
        }
    };

}

void editor::TerrainErosion::hydraulic(std::vector<float>& heights, int width, int height, const TerrainMapRegion& region,
                                       float heightScale, int droplets, uint32_t seed, const FalloffFunc& falloff){
    if (heights.empty() || width <= 2 || height <= 2 || droplets <= 0 || region.empty()){
        return;
    }

    HeightField field{heights, width, height, region};
    const float spanX = static_cast<float>(region.width() - 1);
    const float spanY = static_cast<float>(region.height() - 1);

    // The cut is spread over a fixed neighbourhood, so its shares are the same every step
    float shares[ERODE_TAPS];
    float shareTotal = 0.0f;
    for (int i = 0; i < ERODE_TAPS; i++){
        const int ox = (i % (ERODE_RADIUS * 2 + 1)) - ERODE_RADIUS;
        const int oy = (i / (ERODE_RADIUS * 2 + 1)) - ERODE_RADIUS;
        shares[i] = std::max(0.0f, 1.0f - std::sqrt(static_cast<float>(ox * ox + oy * oy)) / (ERODE_RADIUS + 1.0f));
        shareTotal += shares[i];
    }
    for (float& share : shares){
        share /= shareTotal;
    }

    for (int d = 0; d < droplets; d++){
        float posX = static_cast<float>(region.minX) + TerrainNoise::hash(d, 0, seed) * spanX;
        float posY = static_cast<float>(region.minY) + TerrainNoise::hash(d, 1, seed) * spanY;
        float dirX = 0.0f;
        float dirY = 0.0f;
        float speed = 1.0f;
        float water = 1.0f;
        float sediment = 0.0f;

        if (falloff(static_cast<int>(posX), static_cast<int>(posY)) <= 0.0f){
            continue;
        }

        float restX = posX;
        float restY = posY;

        for (int step = 0; step < DROPLET_STEPS; step++){
            const int cellX = static_cast<int>(posX);
            const int cellY = static_cast<int>(posY);
            const float fracX = posX - static_cast<float>(cellX);
            const float fracY = posY - static_cast<float>(cellY);

            const float h00 = field.at(cellX, cellY);
            const float h10 = field.at(cellX + 1, cellY);
            const float h01 = field.at(cellX, cellY + 1);
            const float h11 = field.at(cellX + 1, cellY + 1);

            const float gradX = (h10 - h00) * (1.0f - fracY) + (h11 - h01) * fracY;
            const float gradY = (h01 - h00) * (1.0f - fracX) + (h11 - h10) * fracX;
            const float currentHeight = (h00 * (1.0f - fracX) + h10 * fracX) * (1.0f - fracY) +
                                        (h01 * (1.0f - fracX) + h11 * fracX) * fracY;

            dirX = dirX * DROPLET_INERTIA - gradX * heightScale * (1.0f - DROPLET_INERTIA);
            dirY = dirY * DROPLET_INERTIA - gradY * heightScale * (1.0f - DROPLET_INERTIA);
            const float length = std::sqrt(dirX * dirX + dirY * dirY);
            if (length < 0.0001f){
                break;
            }
            dirX /= length;
            dirY /= length;
            restX = posX;
            restY = posY;

            posX += dirX;
            posY += dirY;
            if (posX < static_cast<float>(region.minX) || posX > static_cast<float>(region.maxX) ||
                posY < static_cast<float>(region.minY) || posY > static_cast<float>(region.maxY)){
                break;
            }

            const float weight = falloff(static_cast<int>(posX), static_cast<int>(posY));
            if (weight <= 0.0f){
                break;
            }

            const float nextHeight = field.at(static_cast<int>(posX), static_cast<int>(posY));
            const float drop = nextHeight - currentHeight;
            const float capacity = std::max(-drop * heightScale, DROPLET_MIN_SLOPE) * speed * water * DROPLET_CAPACITY;

            // The brush weight scales what actually moves, and the droplet accounts for
            // exactly that, so a stamp neither invents nor loses material at the rim.
            if (sediment > capacity || drop > 0.0f){
                // Uphill fills the pit it came from, never more than the droplet carries
                const float deposit = ((drop > 0.0f) ? std::min(drop, sediment) : (sediment - capacity) * DROPLET_DEPOSIT_RATE) * weight;
                float placed = field.add(cellX, cellY, deposit * (1.0f - fracX) * (1.0f - fracY));
                placed += field.add(cellX + 1, cellY, deposit * fracX * (1.0f - fracY));
                placed += field.add(cellX, cellY + 1, deposit * (1.0f - fracX) * fracY);
                placed += field.add(cellX + 1, cellY + 1, deposit * fracX * fracY);
                sediment -= placed;
            }else{
                const float erosion = std::min((capacity - sediment) * DROPLET_ERODE_RATE, -drop) * weight;
                float cut = 0.0f;
                for (int i = 0; i < ERODE_TAPS; i++){
                    const int ox = (i % (ERODE_RADIUS * 2 + 1)) - ERODE_RADIUS;
                    const int oy = (i / (ERODE_RADIUS * 2 + 1)) - ERODE_RADIUS;
                    cut += field.add(cellX + ox, cellY + oy, -erosion * shares[i]);
                }
                sediment -= cut;
            }

            speed = std::sqrt(std::max(0.0f, speed * speed - drop * heightScale * DROPLET_GRAVITY));
            water *= (1.0f - DROPLET_EVAPORATION);
            if (water < 0.01f){
                break;
            }
        }

        // Whatever the droplet still carries settles where it stopped, so a stamp moves
        // material around instead of removing it from the terrain. Spread over the four
        // texels it sits between: a whole load on one of them builds a spike.
        if (sediment > 0.0f){
            const int restCellX = static_cast<int>(restX);
            const int restCellY = static_cast<int>(restY);
            const float restFracX = restX - static_cast<float>(restCellX);
            const float restFracY = restY - static_cast<float>(restCellY);
            field.add(restCellX, restCellY, sediment * (1.0f - restFracX) * (1.0f - restFracY));
            field.add(restCellX + 1, restCellY, sediment * restFracX * (1.0f - restFracY));
            field.add(restCellX, restCellY + 1, sediment * (1.0f - restFracX) * restFracY);
            field.add(restCellX + 1, restCellY + 1, sediment * restFracX * restFracY);
        }
    }
}

void editor::TerrainErosion::thermal(std::vector<float>& heights, int width, int height, const TerrainMapRegion& region,
                                     float heightScale, float talus, float rate, const FalloffFunc& falloff){
    if (heights.empty() || width <= 2 || height <= 2 || region.empty() || heightScale <= 0.0f || rate <= 0.0f){
        return;
    }

    HeightField field{heights, width, height, region};
    const float limit = talus / heightScale;

    // Neighbours are read from a copy so the pass doesn't depend on texel visit order.
    // Only the stamp is copied: the map itself can be several megabytes.
    const int spanX = region.width();
    const int spanY = region.height();
    std::vector<float> source(static_cast<size_t>(spanX) * static_cast<size_t>(spanY));
    for (int y = 0; y < spanY; y++){
        const float* row = heights.data() + static_cast<size_t>(region.minY + y) * width + region.minX;
        std::copy(row, row + spanX, source.begin() + static_cast<size_t>(y) * spanX);
    }
    auto sourceAt = [&](int x, int y){
        return source[static_cast<size_t>(y - region.minY) * spanX + (x - region.minX)];
    };

    for (int y = region.minY; y <= region.maxY; y++){
        for (int x = region.minX; x <= region.maxX; x++){
            const float weight = falloff(x, y);
            if (weight <= 0.0f){
                continue;
            }

            const float current = sourceAt(x, y);
            const int offsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            float excess[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float total = 0.0f;

            for (int n = 0; n < 4; n++){
                const int nx = x + offsets[n][0];
                const int ny = y + offsets[n][1];
                if (nx < region.minX || ny < region.minY || nx > region.maxX || ny > region.maxY){
                    continue;
                }
                const float difference = current - sourceAt(nx, ny);
                if (difference > limit){
                    excess[n] = difference - limit;
                    total += excess[n];
                }
            }
            if (total <= 0.0f){
                continue;
            }

            const float moved = total * 0.5f * rate * weight;
            for (int n = 0; n < 4; n++){
                if (excess[n] <= 0.0f){
                    continue;
                }
                const float share = moved * (excess[n] / total);
                const float landed = field.add(x + offsets[n][0], y + offsets[n][1], share);
                field.add(x, y, -landed);
            }
        }
    }
}
