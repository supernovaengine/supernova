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

    bufferIndexCount = 0;

    worldWidth = 1024;
    levels = 6;
    resolution = 32;
    rootNodeSize = 500;

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

void Terrain::createPlaneNodeBuffer(int width, int height, int widthSegments, int heightSegments){
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

    bufferIndexCount = 0;

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
}

TerrainNode* Terrain::createNode(float x, float y, float scale, int lodDepth){
    TerrainNode* terrainNode = new TerrainNode(x, y, scale, lodDepth, this);

    this->submeshes.push_back(terrainNode);
    terrainNode->setIndices("indices", bufferIndexCount, 0 * sizeof(unsigned int));

    return terrainNode;
}
/*
void Terrain::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){

    Mesh::updateVPMatrix( viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    if (cameraPosition) {
        offset.x = cameraPosition->x;
        offset.y = cameraPosition->z;
    }
}
*/
bool Terrain::renderLoad(bool shadow){

    if (!shadow){

        instanciateRender();

        render->addProgramDef(S_PROGRAM_IS_TERRAIN);

        //render->addProperty(S_PROPERTY_TERRAINGLOBALOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);

        if (heightData){
            render->addTexture(S_TEXTURESAMPLER_HEIGHTDATA, heightData);
        }

    } else {

        instanciateShadowRender();

        shadowRender->addProgramDef(S_PROGRAM_IS_TERRAIN);

        //shadowRender->addProperty(S_PROPERTY_TERRAINGLOBALOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);

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

    createPlaneNodeBuffer(1, 1, resolution, resolution);

    grid.push_back(createNode(0, 0, rootNodeSize, levels));

    float maxDistance = 1000;
    if (scene && scene->getCamera())
        maxDistance = scene->getCamera()->getFar();

    ranges.resize(levels);
    ranges[levels-1] = maxDistance;
    for (int i = levels-2; i >=0; i--) {
        ranges[i] = ranges[i+1] / 2;
    }

/*
    float initialScale = worldWidth / pow( 2, levels );

    createTile( -initialScale, -initialScale, initialScale );
    createTile( -initialScale, 0, initialScale );
    createTile( 0, 0, initialScale);
    createTile( 0, -initialScale, initialScale );


    for ( float scale = initialScale; scale < worldWidth; scale *= 2 ) {
        createTile( -2 * scale, -2 * scale, scale );
        createTile( -2 * scale, -scale, scale);
        createTile( -2 * scale, 0, scale);
        createTile( -2 * scale, scale, scale);

        createTile( -scale, -2 * scale, scale);
        createTile( -scale, scale, scale);

        createTile( 0, -2 * scale, scale);
        createTile( 0, scale, scale);

        createTile( scale, -2 * scale, scale);
        createTile( scale, -scale, scale);
        createTile( scale, 0, scale);
        createTile( scale, scale, scale);
    }
*/

    return Mesh::load();
}