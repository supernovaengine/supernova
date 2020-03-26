//
// (c) 2020 Eduardo Doria.
//

#include "TerrainNode.h"

using namespace Supernova;

TerrainNode::TerrainNode(float x, float y, float scale, int lodDepth, std::vector<Submesh*>* submeshes): Submesh(){
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
        childs[0] = new TerrainNode(x - quarterScale, y - quarterScale, halfScale, lodDepth - 1, submeshes);
        childs[0]->setVisible(false);
        childs[1] = new TerrainNode(x - quarterScale, y + quarterScale, halfScale, lodDepth - 1, submeshes);
        childs[1]->setVisible(false);
        childs[2] = new TerrainNode(x + quarterScale, y - quarterScale, halfScale, lodDepth - 1, submeshes);
        childs[2]->setVisible(false);
        childs[3] = new TerrainNode(x + quarterScale, y + quarterScale, halfScale, lodDepth - 1, submeshes);
        childs[3]->setVisible(false);

        for (int i = 0; i < 4; i++){
            submeshes->push_back(childs[i]);
        }
    }
}

TerrainNode::~TerrainNode(){

}

const Vector2 &TerrainNode::getOffset() const {
    return offset;
}

void TerrainNode::setOffset(const Vector2 &offset) {
    TerrainNode::offset = offset;
}

float TerrainNode::getScale() const {
    return scale;
}

void TerrainNode::setScale(float scale) {
    TerrainNode::scale = scale;
}

void TerrainNode::setIndices(std::string bufferName, size_t size, size_t offset, DataType type){
    Submesh::setIndices(bufferName, size, offset, type);

    for (int i = 0; i < 4; i++){
        if (childs[i])
            childs[i]->setIndices(bufferName, size, offset, type);
    }
}

bool TerrainNode::renderLoad(bool shadow){

    if (!shadow) {

        render = getSubmeshRender();

        render->addProperty(S_PROPERTY_TERRAINNODEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);
        render->addProperty(S_PROPERTY_TERRAINNODESCALE, S_PROPERTYDATA_FLOAT1, 1, &scale);

    } else {

        shadowRender = getSubmeshShadowRender();

        shadowRender->addProperty(S_PROPERTY_TERRAINNODEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);
        shadowRender->addProperty(S_PROPERTY_TERRAINNODESCALE, S_PROPERTYDATA_FLOAT1, 1, &scale);

    }

    return Submesh::renderLoad(shadow);
}
