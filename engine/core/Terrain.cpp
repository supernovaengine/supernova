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

    heightMap = NULL;
    blendMap = NULL;

    fullResNode = {0,0};
    halfResNode = {0,0};

    autoSetRanges = true;
    heightMapLoaded = false;

    terrainSize = 2000;
    maxHeight = 100;
    rootGridSize = 2;
    levels = 6;
    resolution = 32;

    textureBaseTiles = 1;
    textureDetailTiles = 20;

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Terrain::Terrain(std::string heightMapPath): Terrain(){
    setHeightMap(heightMapPath);
}

Terrain::~Terrain(){
    if (heightMap)
        delete heightMap;

    if (blendMap)
        delete blendMap;

    for (int i = 0; i < textureDetails.size(); i++){
        delete textureDetails[i];
    }

    for (int i = 0; i < grid.size(); i++){
        delete grid[i];
    }
}

Texture* Terrain::getHeightMap(){
    return heightMap;
}

void Terrain::setHeightMap(std::string heightMapPath){
    if (heightMap)
        delete heightMap;

    heightMap = new Texture(heightMapPath);
    heightMap->setPreserveData(true);
    heightMapLoaded = false;
}

Texture* Terrain::getBlendMap(){
    return blendMap;
}

void Terrain::setBlendMap(std::string blendMapPath){
    if (blendMap)
        delete blendMap;

    blendMap = new Texture(blendMapPath);
}

void Terrain::addTextureDetail(std::string heightMapPath, BlendMapColor blendMapColor){
    removeTextureDetail(blendMapColor);

    textureDetails.push_back(new Texture(heightMapPath));
    blendMapColorIndex.push_back(blendMapColor);
}

bool Terrain::removeTextureDetail(BlendMapColor blendMapColor){
    for (int i = 0; i < blendMapColorIndex.size(); i++){
        if (blendMapColorIndex[i] == blendMapColor){
            delete textureDetails[i];
            textureDetails.erase(textureDetails.begin() + i);
            blendMapColorIndex.erase(blendMapColorIndex.begin() + i);
            return true;
        }
    }
    return false;
}

const std::vector<float> &Terrain::getRanges() const {
    return ranges;
}

void Terrain::setRanges(const std::vector<float> &ranges) {
    autoSetRanges = false;
    Terrain::ranges = ranges;
}

int Terrain::getTextureBaseTiles() const {
    return textureBaseTiles;
}

void Terrain::setTextureBaseTiles(int textureBaseTiles) {
    Terrain::textureBaseTiles = textureBaseTiles;
}

int Terrain::getTextureDetailTiles() const {
    return textureDetailTiles;
}

void Terrain::setTextureDetailTiles(int textureDetailTiles) {
    Terrain::textureDetailTiles = textureDetailTiles;
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

float Terrain::getHeight(float x, float y){

    if (x < 0 || y < 0 || x >= terrainSize || y >= terrainSize)
        return 0;

    if (!heightMapLoaded) {
        heightMap->load();
        heightMapLoaded = true;
    }

    TextureData* textureData = heightMap->getTextureData();

    int posX = round(textureData->getWidth() * x / terrainSize);
    int posY = round(textureData->getHeight() * y / terrainSize);

    float val = maxHeight*(textureData->getColorComponent(posX,posY,0)/255.0f);
    return val;
}

float Terrain::maxHeightArea(float x, float z, float w, float h) {
    float maxVal = std::numeric_limits<float>::min();
    for(float i = x; i < x+w; i++)
        for(float j = z; j < z+h; j++){
            float newVal = getHeight(i,j);
            if(newVal > maxVal) {
                maxVal = newVal;
            }
        }
    return maxVal;
}

float Terrain::minHeightArea(float x, float z, float w, float h) {
    float minVal = std::numeric_limits<float>::max();
    for(float i = x; i < x+w; i++)
        for(float j = z; j < z+h; j++){
            float newVal = getHeight(i,j);
            if(newVal < minVal) {
                minVal = newVal;
            }
        }
    return minVal;
}

TerrainNode* Terrain::createNode(float x, float y, float scale, int lodDepth){
    TerrainNode* terrainNode = new TerrainNode(x, y, scale, lodDepth, this);

    return terrainNode;
}

bool Terrain::renderLoad(bool shadow){

    if (!shadow){

        instanciateRender();

        render->addProgramDef(S_PROGRAM_IS_TERRAIN);

        render->addProperty(S_PROPERTY_TERRAINSIZE, S_PROPERTYDATA_FLOAT1, 1, &terrainSize);
        render->addProperty(S_PROPERTY_TERRAINMAXHEIGHT, S_PROPERTYDATA_FLOAT1, 1, &maxHeight);
        render->addProperty(S_PROPERTY_TERRAINRESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

        render->addTexture(S_TEXTURESAMPLER_HEIGHTDATA, heightMap);

        //BlendMaps
        render->addTexture(S_TEXTURESAMPLER_BLENDMAP, blendMap);
        render->addTextureVector(S_TEXTURESAMPLER_TERRAINDETAIL, textureDetails);
        render->addProperty(S_PROPERTY_BLENDMAPCOLORINDEX, S_PROPERTYDATA_INT1, blendMapColorIndex.size(), &blendMapColorIndex.front());

        render->addProperty(S_PROPERTY_TERRAINTEXTUREBASETILES, S_PROPERTYDATA_INT1, 1, &textureBaseTiles);
        render->addProperty(S_PROPERTY_TERRAINTEXTUREDETAILTILES, S_PROPERTYDATA_INT1, 1, &textureDetailTiles);

    } else {

        instanciateShadowRender();

        shadowRender->addProgramDef(S_PROGRAM_IS_TERRAIN);

        shadowRender->addProperty(S_PROPERTY_TERRAINSIZE, S_PROPERTYDATA_FLOAT1, 1, &terrainSize);
        shadowRender->addProperty(S_PROPERTY_TERRAINMAXHEIGHT, S_PROPERTYDATA_FLOAT1, 1, &maxHeight);
        shadowRender->addProperty(S_PROPERTY_TERRAINRESOLUTION, S_PROPERTYDATA_INT1, 1, &resolution);

        shadowRender->addTexture(S_TEXTURESAMPLER_HEIGHTDATA, heightMap);
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

    float rootNodeSize = terrainSize / rootGridSize;

    if (autoSetRanges) {
        float maxDistance = 1000;
        if (scene && scene->getCamera())
            maxDistance = scene->getCamera()->getFar();

        float lastLevel = maxDistance;
        if (maxDistance < (rootNodeSize * 2)){
            lastLevel = rootNodeSize * 2;
            Log::Warn("Terrain quadtree root is not in camera field of view. Increase terrain root grid.");
        }

        ranges.clear();
        ranges.resize(levels);
        ranges[levels - 1] = lastLevel;
        for (int i = levels - 2; i >= 0; i--) {
            ranges[i] = ranges[i + 1] / 2;
        }
    }

    for (int i = 0; i < grid.size(); i++) {
        delete grid[i];
    }
    grid.clear();

    //To center terrain
    float offset = (terrainSize / 2) - (rootNodeSize / 2);

    for (int i = 0; i < rootGridSize; i++) {
        for (int j = 0; j < rootGridSize; j++) {
            float xPos = i*rootNodeSize;
            float zPos = j*rootNodeSize;
            grid.push_back(createNode(xPos-offset, zPos-offset, rootNodeSize, levels));
        }
    }

    if (heightMap)
        heightMapLoaded = true;

    return Mesh::load();
}
