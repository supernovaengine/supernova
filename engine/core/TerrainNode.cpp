//
// (c) 2020 Eduardo Doria.
//

#include "TerrainNode.h"
#include "Terrain.h"
#include "Scene.h"

using namespace Supernova;

TerrainNode::TerrainNode(float x, float y, float size, int lodDepth, Terrain* terrain): Submesh(){
    this->position = Vector2(x, y);
    this->size = size;
    this->terrain = terrain;

    this->currentRange = 0;
    this->resolution = terrain->resolution;

    if (lodDepth == 1){
        childs[0] = NULL;
        childs[1] = NULL;
        childs[2] = NULL;
        childs[3] = NULL;
    }else{
        float halfSize = size/2;
        float quarterSize = halfSize/2;
        childs[0] = new TerrainNode(x - quarterSize, y - quarterSize, halfSize, lodDepth - 1, terrain);
        childs[0]->setVisible(false);
        childs[1] = new TerrainNode(x - quarterSize, y + quarterSize, halfSize, lodDepth - 1, terrain);
        childs[1]->setVisible(false);
        childs[2] = new TerrainNode(x + quarterSize, y - quarterSize, halfSize, lodDepth - 1, terrain);
        childs[2]->setVisible(false);
        childs[3] = new TerrainNode(x + quarterSize, y + quarterSize, halfSize, lodDepth - 1, terrain);
        childs[3]->setVisible(false);

        for (int i = 0; i < 4; i++){
            terrain->submeshes.push_back(childs[i]);
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

bool TerrainNode::LODSelect(std::vector<float> &ranges, int lodLevel){
    currentRange = ranges[lodLevel];

    AlignedBox box = getAlignedBox();

    if (!inSphere(ranges[lodLevel], box)) {
        return false;
    }

    if (!inFrustum(box)) {
        return true;
    }

    if( lodLevel == 0 ) {
        //Full resolution
        this->resolution = terrain->resolution;
        this->setIndices("indices", terrain->fullResNode.indexCount, terrain->fullResNode.indexOffset * sizeof(unsigned int));
        this->setVisible(true);

        return true;
    } else {

        if( !inSphere(ranges[lodLevel-1], box) ) {
            //Full resolution
            this->resolution = terrain->resolution;
            this->setIndices("indices", terrain->fullResNode.indexCount, terrain->fullResNode.indexOffset * sizeof(unsigned int));
            this->setVisible(true);
        } else {
            TerrainNode *child;
            for (int i = 0; i < 4; i++) {
                child = childs[i];
                if (!child->LODSelect(ranges, lodLevel - 1)) {
                    //Half resolution
                    child->resolution = terrain->resolution / 2;
                    child->setIndices("indices", terrain->halfResNode.indexCount, terrain->halfResNode.indexOffset * sizeof(unsigned int));
                    child->currentRange = currentRange;
                    child->setVisible(true);
                }
            }
        }
        return true;
    }
}

AlignedBox TerrainNode::getAlignedBox(){
    float halfSize = size/2;
    Vector3 worldHalfScale(halfSize * terrain->getWorldScale().x, 1, halfSize * terrain->getWorldScale().z);
    Vector3 worldPosition = terrain->getModelMatrix() * Vector3(position.x, 0, position.y);

    //TODO: minHeight and maxHeight
    Vector3 c1 = Vector3(worldPosition.x - worldHalfScale.x, 0, worldPosition.z - worldHalfScale.z);
    Vector3 c2 = Vector3(worldPosition.x + worldHalfScale.x, 100, worldPosition.z + worldHalfScale.z);

    return AlignedBox(c1, c2);
};

bool TerrainNode::inFrustum(const AlignedBox& box){
    if (terrain->getScene() && terrain->getScene()->getCamera()) {
        Camera* camera = terrain->getScene()->getCamera();

        return camera->isInside(box);
    }

    return false;
}

bool TerrainNode::inSphere(float radius, const AlignedBox& box) {
    if (terrain->getScene()) {
        float r2 = radius*radius;
        Vector3 cameraPosition = terrain->getScene()->getCameraPosition();

        //TODO: minHeight and maxHeight
        Vector3 c1 = box.getMinimum();
        Vector3 c2 = box.getMaximum();
        Vector3 distV;

        if (cameraPosition.x < c1.x) distV.x = (cameraPosition.x - c1.x);
        else if (cameraPosition.x > c2.x) distV.x = (cameraPosition.x - c2.x);
        if (cameraPosition.y < c1.y) distV.y = (cameraPosition.y - c1.y);
        else if (cameraPosition.y > c2.y) distV.y = (cameraPosition.y - c2.y);
        if (cameraPosition.z < c1.z) distV.z = (cameraPosition.z - c1.z);
        else if (cameraPosition.z > c2.z) distV.z = (cameraPosition.z - c2.z);

        float dist2 = distV.dotProduct(distV);

        return dist2 <= r2;
    }

    return false;
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
        render->addProperty(S_PROPERTY_TERRAINNODERANGE, S_PROPERTYDATA_FLOAT1, 1, &currentRange);
        render->addProperty(S_PROPERTY_TERRAINNODERESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

    } else {

        shadowRender = getSubmeshShadowRender();

        shadowRender->addProperty(S_PROPERTY_TERRAINNODEPOS, S_PROPERTYDATA_FLOAT2, 1, &position);
        shadowRender->addProperty(S_PROPERTY_TERRAINNODESIZE, S_PROPERTYDATA_FLOAT1, 1, &size);
        shadowRender->addProperty(S_PROPERTY_TERRAINNODERANGE, S_PROPERTYDATA_FLOAT1, 1, &currentRange);
        shadowRender->addProperty(S_PROPERTY_TERRAINNODERESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

    }

    return Submesh::renderLoad(shadow);
}
