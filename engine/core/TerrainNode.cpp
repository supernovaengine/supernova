//
// (c) 2020 Eduardo Doria.
//

#include "TerrainNode.h"

using namespace Supernova;

TerrainNode::TerrainNode(float x, float y, float size, int lodDepth, std::vector<Submesh*> &submeshes): Submesh(){
    this->position = Vector2(x, y);
    this->size = size;

    if (lodDepth == 1){
        childs[0] = NULL;
        childs[1] = NULL;
        childs[2] = NULL;
        childs[3] = NULL;
    }else{
        float halfSize = size/2;
        float quarterSize = halfSize/2;
        childs[0] = new TerrainNode(x - quarterSize, y - quarterSize, halfSize, lodDepth - 1, submeshes);
        childs[0]->setVisible(false);
        childs[1] = new TerrainNode(x - quarterSize, y + quarterSize, halfSize, lodDepth - 1, submeshes);
        childs[1]->setVisible(false);
        childs[2] = new TerrainNode(x + quarterSize, y - quarterSize, halfSize, lodDepth - 1, submeshes);
        childs[2]->setVisible(false);
        childs[3] = new TerrainNode(x + quarterSize, y + quarterSize, halfSize, lodDepth - 1, submeshes);
        childs[3]->setVisible(false);

        for (int i = 0; i < 4; i++){
            submeshes.push_back(childs[i]);
        }
    }
}

TerrainNode::~TerrainNode(){

}

const Vector2 &TerrainNode::getPosition() const {
    return position;
}

void TerrainNode::setPosition(const Vector2 &position) {
    TerrainNode::position = position;
}

float TerrainNode::getSize() const {
    return size;
}

void TerrainNode::setSize(float size) {
    TerrainNode::size = size;
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

        render->addProperty(S_PROPERTY_TERRAINNODEPOS, S_PROPERTYDATA_FLOAT2, 1, &position);
        render->addProperty(S_PROPERTY_TERRAINNODESIZE, S_PROPERTYDATA_FLOAT1, 1, &size);

    } else {

        shadowRender = getSubmeshShadowRender();

        shadowRender->addProperty(S_PROPERTY_TERRAINNODEPOS, S_PROPERTYDATA_FLOAT2, 1, &position);
        shadowRender->addProperty(S_PROPERTY_TERRAINNODESIZE, S_PROPERTYDATA_FLOAT1, 1, &size);

    }

    return Submesh::renderLoad(shadow);
}
