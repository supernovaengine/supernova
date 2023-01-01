//
// (c) 2022 Eduardo Doria.
//

#include "Terrain.h"
#include "util/Color.h"

using namespace Supernova;

Terrain::Terrain(Scene* scene): Object(scene){
    addComponent<TerrainComponent>({});
}

Terrain::~Terrain(){

}

void Terrain::setHeightMap(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setHeightMap(Framebuffer* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setFramebuffer(framebuffer);

    terrain.needUpdateTexture = true;
}

void Terrain::setBlendMap(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.blendMap.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setBlendMap(Framebuffer* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.blendMap.setFramebuffer(framebuffer);

    terrain.needUpdateTexture = true;
}

void Terrain::setTextureDetailRed(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.textureDetailRed.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTextureDetailGreen(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.textureDetailGreen.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTextureDetailBlue(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.textureDetailBlue.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTexture(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.material.baseColorTexture.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTexture(Framebuffer* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.material.baseColorTexture.setFramebuffer(framebuffer);

    terrain.needUpdateTexture = true;
}

void Terrain::setColor(Vector4 color){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.material.baseColorFactor = Color::sRGBToLinear(color);
}

void Terrain::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Terrain::getColor() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return Color::linearTosRGB(terrain.material.baseColorFactor);
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

void Terrain::setResolution(int resolution){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.resolution != resolution){
        terrain.resolution = resolution;

        terrain.needUpdateTerrain = true;
        terrain.needReload = true;
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
    Transform& transform = getComponent<Transform>();

    if (terrain.rootGridSize != rootGridSize){
        terrain.rootGridSize = rootGridSize;

        terrain.needUpdateTerrain = true;
        terrain.needReload = true;
        transform.needUpdate = true;
    }
}

int Terrain::getRootGridSize() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.rootGridSize;
}

void Terrain::setLevels(int levels){
    TerrainComponent& terrain = getComponent<TerrainComponent>();
    Transform& transform = getComponent<Transform>();

    if (terrain.levels != levels){
        terrain.levels = levels;

        terrain.needUpdateTerrain = true;
        terrain.needReload = true;
        transform.needUpdate = true;
    }
}

int Terrain::getLevels() const{
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return (int)terrain.levels;
}