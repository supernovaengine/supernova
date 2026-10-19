// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Terrain.h"
#include "util/Color.h"

#include "subsystem/MeshSystem.h"
#include "subsystem/RenderSystem.h"

using namespace doriax;

Terrain::Terrain(Scene* scene): Mesh(scene){
    addComponent<TerrainComponent>();
}

Terrain::Terrain(Scene* scene, Entity entity): Mesh(scene, entity){
}

Terrain::~Terrain(){
}

bool Terrain::createTerrain(){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    MeshComponent& mesh = getComponent<MeshComponent>();
    Transform& transform = getComponent<Transform>();

    return scene->getSystem<MeshSystem>()->createOrUpdateTerrain(terrain, mesh, transform);
}

void Terrain::setHeightMap(const std::string& path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setHeightMap(Framebuffer* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setFramebuffer(framebuffer);

    terrain.needUpdateTexture = true;
}

void Terrain::setBlendMap(const std::string& path){
    setBlendMap(0, path);
}

void Terrain::setBlendMap(Framebuffer* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (terrain.blendMaps.empty()){
        terrain.blendMaps.resize(1);
    }
    terrain.blendMaps[0].setFramebuffer(framebuffer);

    terrain.needUpdateTexture = true;
}

void Terrain::setBlendMap(unsigned int index, const std::string& path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (index >= MAX_TERRAIN_BLENDMAPS){
        Log::error("Terrain has no blend map %u, the limit is %d", index, MAX_TERRAIN_BLENDMAPS);
        return;
    }
    if (index >= terrain.blendMaps.size()){
        terrain.blendMaps.resize(index + 1);
    }
    terrain.blendMaps[index].setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTextureLayer(unsigned int index, const std::string& path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (index >= MAX_TERRAIN_LAYERS){
        Log::error("Terrain has no layer %u, the limit is %d", index, MAX_TERRAIN_LAYERS);
        return;
    }
    if (index >= terrain.surfaceLayers.size()){
        terrain.surfaceLayers.resize(index + 1);
    }
    terrain.surfaceLayers[index].colorTexture.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setSurfaceLayer(unsigned int index, const TerrainSurfaceLayer& layer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (index >= MAX_TERRAIN_LAYERS){
        Log::error("Terrain has no layer %u, the limit is %d", index, MAX_TERRAIN_LAYERS);
        return;
    }
    if (index >= terrain.surfaceLayers.size()){
        terrain.surfaceLayers.resize(index + 1);
    }
    terrain.surfaceLayers[index] = layer;

    terrain.needUpdateTexture = true;
}

TerrainSurfaceLayer Terrain::getSurfaceLayer(unsigned int index) const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (index >= terrain.surfaceLayers.size()){
        Log::error("Terrain has no layer %u", index);
        return TerrainSurfaceLayer();
    }

    return terrain.surfaceLayers[index];
}

// Emission, alpha and secondary UV sets have no meaning on a terrain layer and are dropped
void Terrain::setLayerFromMaterial(unsigned int index, const Material& material){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    const TerrainSurfaceLayer previous = (index < terrain.surfaceLayers.size()) ? terrain.surfaceLayers[index] : TerrainSurfaceLayer();

    setSurfaceLayer(index, terrainLayerFromMaterial(material, previous));
}

// Blend map channels are positional: every later layer moves down one channel, onto the
// paint the removed one leaves behind. Only dropping the last layer is lossless.
void Terrain::removeSurfaceLayer(unsigned int index){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (index >= terrain.surfaceLayers.size()){
        Log::error("Terrain has no layer %u", index);
        return;
    }
    terrain.surfaceLayers.erase(terrain.surfaceLayers.begin() + index);

    terrain.needUpdateTexture = true;
}

unsigned int Terrain::getNumLayers() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (unsigned int)terrain.surfaceLayers.size();
}

// The first three layers, under the names they had when a blend map was the only one
void Terrain::setTextureDetailRed(const std::string& path){
    setTextureLayer(0, path);
}

void Terrain::setTextureDetailGreen(const std::string& path){
    setTextureLayer(1, path);
}

void Terrain::setTextureDetailBlue(const std::string& path){
    setTextureLayer(2, path);
}

void Terrain::setSize(float size){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.terrainSize != size){
        terrain.terrainSize = size;

        terrain.needUpdateTerrain = true;
        transform.needUpdate = true;
    }
}

float Terrain::getSize() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return terrain.terrainSize;
}

void Terrain::setMaxHeight(float maxHeight){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    if (terrain.maxHeight != maxHeight){
        terrain.maxHeight = maxHeight;
        terrain.needUpdateTerrain = true;
    }
}

float Terrain::getMaxHeight() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return terrain.maxHeight;
}

void Terrain::setFoliageDistanceScale(float scale){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.foliageDistanceScale = std::max(0.0f, scale);
}

float Terrain::getFoliageDistanceScale() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return terrain.foliageDistanceScale;
}

void Terrain::setResolution(int resolution){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    MeshComponent& mesh = getComponent<MeshComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.resolution != resolution){
        terrain.resolution = resolution;

        terrain.needUpdateTerrain = true;
        mesh.needReload = true;
        transform.needUpdate = true;
    }
}

int Terrain::getResolution() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.resolution;
}

void Terrain::setTextureBaseTiles(int textureBaseTiles){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.textureBaseTiles = textureBaseTiles; // no need update terrain
}

int Terrain::getTextureBaseTiles() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.textureBaseTiles;
}

void Terrain::setTextureDetailTiles(int textureDetailTiles){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.textureDetailTiles = textureDetailTiles; // no need update terrain
}

int Terrain::getTextureDetailTiles() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.textureDetailTiles;
}

void Terrain::setRootGridSize(int rootGridSize){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    MeshComponent& mesh = getComponent<MeshComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.rootGridSize != rootGridSize){
        terrain.rootGridSize = rootGridSize;

        terrain.needUpdateTerrain = true;
        mesh.needReload = true;
        transform.needUpdate = true;
    }
}

int Terrain::getRootGridSize() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.rootGridSize;
}

void Terrain::setLevels(int levels){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    MeshComponent& mesh = getComponent<MeshComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.levels != levels){
        terrain.levels = levels;

        terrain.needUpdateTerrain = true;
        mesh.needReload = true;
        transform.needUpdate = true;
    }
}

int Terrain::getLevels() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.levels;
}