// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "ecs/Entity.h"
#include "texture/Texture.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace doriax::editor {
class Project;
}

namespace doriax::editor::ai {

struct TerrainHeightmapRequest {
    int resolution = 512;
    std::string mode = "middle";
    uint32_t seed = 0;
    float baseHeight = 0.5f;
    float amplitude = 0.35f;
    float frequency = 4.0f;
    int octaves = 4;
};

struct TerrainHeightmapResult {
    bool success = false;
    std::string error;
    std::string relativePath;
    Texture texture;
};

struct TerrainBlendmapRequest {
    int resolution = 512;
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

struct TerrainBlendmapResult {
    bool success = false;
    std::string error;
    std::string relativePath;
    std::filesystem::path fullPath;
};

class AiTerrainCapability {
public:
    static TerrainHeightmapResult createHeightmap(Project* project,
                                                  uint32_t sceneId,
                                                  Entity entity,
                                                  const TerrainHeightmapRequest& request);

    static TerrainBlendmapResult createBlendmap(Project* project,
                                                uint32_t sceneId,
                                                Entity entity,
                                                const TerrainBlendmapRequest& request);
};

} // namespace doriax::editor::ai
