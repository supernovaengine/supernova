//
// (c) 2022 Eduardo Doria.
//

#include "Terrain.h"
#include "math/Color.h"

using namespace Supernova;

Terrain::Terrain(Scene* scene): Object(scene){
    addComponent<TerrainComponent>({});
}

Terrain::~Terrain(){

}

void Terrain::setHeightMap(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setPath(path);

    //mesh.submeshes[0].needUpdateTexture = true;
}

void Terrain::setHeightMap(FramebufferRender* framebuffer){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.heightMap.setFramebuffer(framebuffer);

    //mesh.submeshes[0].needUpdateTexture = true;
}

void Terrain::setTexture(std::string path){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    terrain.material.baseColorTexture.setPath(path);

    terrain.needUpdateTexture = true;
}

void Terrain::setTexture(FramebufferRender* framebuffer){
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

Vector4 Terrain::getColor(){
    TerrainComponent& terrain = getComponent<TerrainComponent>();

    return Color::linearTosRGB(terrain.material.baseColorFactor);
}