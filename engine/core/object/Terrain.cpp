//
// (c) 2022 Eduardo Doria.
//

#include "Terrain.h"

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