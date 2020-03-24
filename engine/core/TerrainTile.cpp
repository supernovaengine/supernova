//
// (c) 2020 Eduardo Doria.
//

#include "TerrainTile.h"

using namespace Supernova;

TerrainTile::TerrainTile(float x, float y, float scale, int lodDepth, std::vector<Submesh*>* submeshes): Submesh(){
    this->offset = Vector2(x, y);
    this->scale = scale;

    if (lodDepth == 1){
        childs[0] = NULL;
        childs[1] = NULL;
        childs[2] = NULL;
        childs[3] = NULL;
    }else{
        float halfScale = scale/2;
        float quarterScale = halfScale/2;
        childs[0] = new TerrainTile(x-quarterScale, y-quarterScale, halfScale, lodDepth-1, submeshes);
        childs[0]->setVisible(false);
        childs[1] = new TerrainTile(x-quarterScale, y+quarterScale, halfScale, lodDepth-1, submeshes);
        childs[1]->setVisible(false);
        childs[2] = new TerrainTile(x+quarterScale, y-quarterScale, halfScale, lodDepth-1, submeshes);
        childs[2]->setVisible(false);
        childs[3] = new TerrainTile(x+quarterScale, y+quarterScale, halfScale, lodDepth-1, submeshes);
        childs[3]->setVisible(false);

        for (int i = 0; i < 4; i++){
            submeshes->push_back(childs[i]);
        }
    }
}

TerrainTile::~TerrainTile(){

}

const Vector2 &TerrainTile::getOffset() const {
    return offset;
}

void TerrainTile::setOffset(const Vector2 &offset) {
    TerrainTile::offset = offset;
}

float TerrainTile::getScale() const {
    return scale;
}

void TerrainTile::setScale(float scale) {
    TerrainTile::scale = scale;
}

void TerrainTile::setIndices(std::string bufferName, size_t size, size_t offset, DataType type){
    Submesh::setIndices(bufferName, size, offset, type);

    for (int i = 0; i < 4; i++){
        if (childs[i])
            childs[i]->setIndices(bufferName, size, offset, type);
    }
}

bool TerrainTile::renderLoad(bool shadow){

    if (!shadow) {

        render = getSubmeshRender();

        render->addProperty(S_PROPERTY_TERRAINTILEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);
        render->addProperty(S_PROPERTY_TERRAINTILESCALE, S_PROPERTYDATA_FLOAT1, 1, &scale);

    } else {

        shadowRender = getSubmeshShadowRender();

        shadowRender->addProperty(S_PROPERTY_TERRAINTILEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);
        shadowRender->addProperty(S_PROPERTY_TERRAINTILESCALE, S_PROPERTYDATA_FLOAT1, 1, &scale);

    }

    return Submesh::renderLoad(shadow);
}
