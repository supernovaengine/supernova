// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#include "AiTerrainCapability.h"

#include "AiPathUtils.h"
#include "Project.h"
#include "pool/TextureDataPool.h"
#include "texture/TextureData.h"
#include "util/PngWriter.h"
#include "util/TerrainNoise.h"

#include "stb_image_write.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>

namespace fs = std::filesystem;

namespace doriax::editor::ai {

namespace {

constexpr int kHeightmapChannels = 1;
constexpr ColorFormat kHeightmapFormat = ColorFormat::RED16;

void encodeHeight16(std::vector<unsigned char>& pixels, size_t texelIndex, float value) {
    const float clamped = std::max(0.0f, std::min(1.0f, value));
    const unsigned int quantized = static_cast<unsigned int>(std::lround(clamped * 65535.0f));
    const size_t byteIndex = texelIndex * 2;
    pixels[byteIndex] = static_cast<unsigned char>(quantized & 0xFF);
    pixels[byteIndex + 1] = static_cast<unsigned char>((quantized >> 8) & 0xFF);
}

std::vector<unsigned char> makeHeightPixels(const TerrainHeightmapRequest& request) {
    const int resolution = std::max(2, request.resolution);
    const size_t texelCount = static_cast<size_t>(resolution) * static_cast<size_t>(resolution);
    std::vector<unsigned char> pixels(texelCount * 2, 0);
    float baseHeight = std::max(0.0f, std::min(1.0f, request.baseHeight));
    float amplitude = std::max(0.0f, std::min(1.0f, request.amplitude));
    float frequency = std::max(0.01f, request.frequency);
    int octaves = std::max(1, std::min(8, request.octaves));

    std::mt19937 rng(request.seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            float h = baseHeight;
            if (request.mode == "random_noise") {
                h = baseHeight + dist(rng) * amplitude;
            } else if (request.mode == "fractal_noise") {
                float sum = 0.0f;
                float weight = 1.0f;
                float weightSum = 0.0f;
                float f = frequency;
                for (int o = 0; o < octaves; ++o) {
                    const float nx = (static_cast<float>(x) / static_cast<float>(resolution - 1)) * f;
                    const float ny = (static_cast<float>(y) / static_cast<float>(resolution - 1)) * f;
                    sum += (TerrainNoise::value(nx, ny, request.seed + static_cast<uint32_t>(o * 1013)) * 2.0f - 1.0f) * weight;
                    weightSum += weight;
                    weight *= 0.5f;
                    f *= 2.0f;
                }
                h = baseHeight + (weightSum > 0.0f ? sum / weightSum : 0.0f) * amplitude;
            }
            encodeHeight16(pixels, static_cast<size_t>(y) * resolution + x, h);
        }
    }
    return pixels;
}

std::string makeTerrainHeightmapPath(Project* project, uint32_t sceneId, Entity entity, uint32_t seed) {
    fs::path baseDir = project->getTerrainMapsDir();
    std::error_code ec;
    fs::create_directories(baseDir, ec);

    for (uint32_t i = 0; i < 10000; ++i) {
        fs::path candidate = baseDir / ("terrain_ai_" + std::to_string(sceneId) + "_" +
                                        std::to_string(entity) + "_height_" +
                                        std::to_string(seed) + "_" + std::to_string(i) + ".png");
        if (!fs::exists(candidate, ec)) {
            fs::path rel = fs::relative(candidate, project->getAssetsPath(), ec);
            return ec ? candidate.generic_string() : rel.generic_string();
        }
    }

    fs::path fallback = baseDir / ("terrain_ai_" + std::to_string(sceneId) + "_" +
                                   std::to_string(entity) + "_height.png");
    fs::path rel = fs::relative(fallback, project->getAssetsPath(), ec);
    return ec ? fallback.generic_string() : rel.generic_string();
}

bool setFileBackedHeightTexture(Project* project, Texture& texture, const std::string& relativePath,
                                int resolution, const std::vector<unsigned char>& pixels) {
    const int bytesPerChannel = TextureData::getBytesPerChannel(kHeightmapFormat);
    const size_t expectedSize = static_cast<size_t>(resolution) * static_cast<size_t>(resolution) *
                                static_cast<size_t>(kHeightmapChannels) * static_cast<size_t>(bytesPerChannel);
    if (!project || project->getProjectPath().empty() || relativePath.empty() ||
        resolution <= 0 || pixels.size() < expectedSize) {
        return false;
    }

    fs::path outputPath = project->resolveAssetPath(relativePath);

    std::error_code ec;
    fs::create_directories(outputPath.parent_path(), ec);
    if (ec || !writeGray16Png(outputPath, resolution, resolution, pixels.data(), pixels.size())) {
        return false;
    }

    texture.setPath(relativePath);
    texture.setReleaseDataAfterLoad(false);

    unsigned char* raw = static_cast<unsigned char*>(std::malloc(expectedSize));
    if (!raw) {
        return false;
    }
    std::memcpy(raw, pixels.data(), expectedSize);

    TextureData seed(resolution, resolution, static_cast<unsigned int>(expectedSize),
                     kHeightmapFormat, kHeightmapChannels, raw);
    seed.setDataOwned(false);

    std::array<TextureData, 6> arr;
    arr[0] = seed;
    auto poolEntry = TextureDataPool::get(relativePath, arr);
    const bool seedAccepted = poolEntry && poolEntry->at(0).getData() == raw;
    if (!seedAccepted) {
        std::free(raw);
    }

    TextureLoadResult result = texture.load();
    if (result.state != ResourceLoadState::Finished || !result.data || !texture.getData().getData()) {
        return false;
    }
    if (texture.getData().getChannels() != kHeightmapChannels ||
        texture.getData().getColorFormat() != kHeightmapFormat) {
        return false;
    }
    if (seedAccepted) {
        texture.getData().setDataOwned(true);
    }
    return true;
}

unsigned char blendChannel(float value) {
    return static_cast<unsigned char>(std::lround(std::max(0.0f, std::min(1.0f, value)) * 255.0f));
}

} // namespace

TerrainHeightmapResult AiTerrainCapability::createHeightmap(Project* project,
                                                            uint32_t sceneId,
                                                            Entity entity,
                                                            const TerrainHeightmapRequest& request) {
    TerrainHeightmapResult result;
    if (!project) {
        result.error = "No project is available.";
        return result;
    }

    const int resolution = std::max(2, request.resolution);
    std::vector<unsigned char> pixels = makeHeightPixels(request);
    result.relativePath = makeTerrainHeightmapPath(project, sceneId, entity, request.seed);

    if (!setFileBackedHeightTexture(project, result.texture, result.relativePath, resolution, pixels)) {
        result.error = "Failed to write generated terrain heightmap.";
        return result;
    }

    result.success = true;
    return result;
}

TerrainBlendmapResult AiTerrainCapability::createBlendmap(Project* project,
                                                          uint32_t sceneId,
                                                          Entity entity,
                                                          const TerrainBlendmapRequest& request) {
    TerrainBlendmapResult result;
    if (!project) {
        result.error = "No project is available.";
        return result;
    }

    const int resolution = std::max(2, request.resolution);
    const unsigned char r = blendChannel(request.red);
    const unsigned char g = blendChannel(request.green);
    const unsigned char b = blendChannel(request.blue);

    std::vector<unsigned char> pixels(static_cast<size_t>(resolution) * static_cast<size_t>(resolution) * 4);
    for (size_t i = 0; i < pixels.size(); i += 4) {
        pixels[i] = r;
        pixels[i + 1] = g;
        pixels[i + 2] = b;
        pixels[i + 3] = 255;
    }

    fs::path outputDir = project->getTerrainMapsDir();
    std::error_code ec;
    fs::create_directories(outputDir, ec);
    if (ec) {
        result.error = "Failed to create terrain maps directory: " + ec.message();
        return result;
    }

    result.fullPath = PathUtils::uniqueChildPath(
        outputDir,
        "terrain_ai_" + std::to_string(sceneId) + "_" + std::to_string(entity) + "_blend",
        ".png");
    if (!stbi_write_png(result.fullPath.string().c_str(), resolution, resolution, 4, pixels.data(), resolution * 4)) {
        result.error = "Failed to write generated terrain blendmap.";
        return result;
    }

    fs::path rel = fs::relative(result.fullPath, project->getAssetsPath(), ec);
    result.relativePath = (ec ? result.fullPath : rel).generic_string();
    result.success = true;
    return result;
}

} // namespace doriax::editor::ai
