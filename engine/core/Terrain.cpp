//
// (c) 2019 Eduardo Doria.
//

#include "image/TextureLoader.h"
#include "Terrain.h"
#include "Log.h"
#include "Scene.h"
#include <math.h>

using namespace Supernova;

Terrain::Terrain(): Mesh(){

    primitiveType = S_PRIMITIVE_TRIANGLES;

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    heightData = NULL;

    fullResNode = {0,0};
    halfResNode = {0,0};

    terrainSize = 2000;
    levels = 4;
    resolution = 32;
    rootNodeSize = 1000;

    //float leafNodeSize = 1.0f;
    //float rootNodeSize = leafNodeSize*pow(2, levels-1);

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Terrain::Terrain(std::string heightMapPath): Terrain(){
    setHeightmap(heightMapPath);
}

Terrain::~Terrain(){
    if (heightData)
        delete heightData;
}

Texture* Terrain::getHeightmap(){
    return heightData;
}

void Terrain::setHeightmap(std::string heightMapPath){
    heightData = new Texture(heightMapPath);
}

Terrain::NodeIndex Terrain::createPlaneNodeBuffer(int width, int height, int widthSegments, int heightSegments){
    float width_half = (float)width / 2;
    float height_half = (float)height / 2;

    int gridX = widthSegments;
    int gridY = heightSegments;

    int gridX1 = gridX + 1;
    int gridY1 = gridY + 1;

    float segment_width = (float)width / gridX;
    float segment_height = (float)height / gridY;

    Attribute* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    Attribute* attTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    Attribute* attNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);
    Attribute* attIndice = indices.getAttribute(S_INDEXATTRIBUTE);

    int bufferCount = buffer.getCount();

    for (int iy = 0; iy < gridY1; iy++) {
        float y = iy * segment_height - height_half;
        for (int ix = 0; ix < gridX1; ix ++) {

            float x = ix * segment_width - width_half;

            buffer.addVector3(attVertex, Vector3(x, 0, -y));
            buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
            buffer.addVector2(attTexcoord, Vector2((float)ix / gridX, (float)iy / gridY));

        }
    }

    unsigned int bufferIndexCount = 0;
    unsigned int bufferIndexOffset = indices.getCount();

    for (int iy = 0; iy < gridY; iy++) {
        for (int ix = 0; ix < gridX; ix++) {

            int a = ix + gridX1 * iy;
            int b = ix + gridX1 * ( iy + 1 );
            int c = ( ix + 1 ) + gridX1 * ( iy + 1 );
            int d = ( ix + 1 ) + gridX1 * iy;

            indices.addUInt(attIndice, a + bufferCount);
            indices.addUInt(attIndice, b + bufferCount);
            indices.addUInt(attIndice, d + bufferCount);

            indices.addUInt(attIndice, b + bufferCount);
            indices.addUInt(attIndice, c + bufferCount);
            indices.addUInt(attIndice, d + bufferCount);

            bufferIndexCount += 6;

        }
    }

    return {bufferIndexCount, bufferIndexOffset};
}

TerrainNode* Terrain::createNode(float x, float y, float scale, int lodDepth){
    TerrainNode* terrainNode = new TerrainNode(x, y, scale, lodDepth, this);

    this->submeshes.push_back(terrainNode);

    return terrainNode;
}

bool Terrain::renderLoad(bool shadow){

    if (!shadow){

        instanciateRender();

        render->addProgramDef(S_PROGRAM_IS_TERRAIN);

        render->addProperty(S_PROPERTY_TERRAINSIZE, S_PROPERTYDATA_FLOAT1, 1, &terrainSize);
        render->addProperty(S_PROPERTY_TERRAINRESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

        if (heightData){
            render->addTexture(S_TEXTURESAMPLER_HEIGHTDATA, heightData);
        }

    } else {

        instanciateShadowRender();

        shadowRender->addProgramDef(S_PROGRAM_IS_TERRAIN);

        shadowRender->addProperty(S_PROPERTY_TERRAINSIZE, S_PROPERTYDATA_FLOAT1, 1, &terrainSize);
        shadowRender->addProperty(S_PROPERTY_TERRAINRESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

        if (heightData){
            shadowRender->addTexture(S_TEXTURESAMPLER_HEIGHTDATA, heightData);
        }
    }

    return Mesh::renderLoad(shadow);
};

void Terrain::updateNodes(){
    if (scene && scene->getCamera()) {
        for (int i = 0; i < submeshes.size(); i++) {
            submeshes[i]->setVisible(false);
        }
        for (int i = 0; i < grid.size(); i++) {
            grid[i]->LODSelect(ranges, levels - 1);
        }
    }
}

void Terrain::updateModelMatrix(){
    Mesh::updateModelMatrix();

    updateNodes();
}

void Terrain::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Mesh::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    updateNodes();
}


bool Terrain::load(){

    fullResNode = createPlaneNodeBuffer(1, 1, resolution, resolution);
    halfResNode = createPlaneNodeBuffer(1, 1, resolution/2, resolution/2);

    float maxDistance = 1000;
    if (scene && scene->getCamera())
        maxDistance = scene->getCamera()->getFar();

    ranges.resize(levels);
    ranges[levels-1] = maxDistance;
    for (int i = levels-2; i >=0; i--) {
        ranges[i] = ranges[i+1] / 2;
    }

    rootNodeSize = maxDistance / 2;
    //TODO: Check if rootNodeSize is bigger than terrainSize

    //TODO: Clear grid
    int gridWidth = floor(terrainSize/rootNodeSize);
    int gridHeight = floor(terrainSize/rootNodeSize);

    for (int i = 0; i < gridWidth; i++) {
        for (int j = 0; j < gridHeight; j++) {
            float xPos = i*rootNodeSize;
            float zPos = j*rootNodeSize;
            grid.push_back(createNode(xPos-(terrainSize/4), zPos-(terrainSize/4), rootNodeSize, levels));
        }
    }


    return Mesh::load();
}