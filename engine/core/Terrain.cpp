//
// (c) 2019 Eduardo Doria.
//

#include "image/TextureLoader.h"
#include "Terrain.h"
#include "Log.h"

using namespace Supernova;

Terrain::Terrain(): Mesh(){
    heightmap_data = NULL;
    heightmap_path = "";

    primitiveType = S_PRIMITIVE_TRIANGLE_STRIP;

    buffers["vertices"] = &buffer;

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Terrain::Terrain(std::string heightmap): Terrain(){
    this->heightmap_path = heightmap;
}

Terrain::~Terrain(){
    if (heightmap_data)
        delete heightmap_data;
}

bool Terrain::loadTerrainFromImage(std::string path) {
    if (!path.empty()) {
        TextureLoader texLoader;
        heightmap_data = texLoader.loadTextureData(path);
    }

    if (!heightmap_data)
        return false;

    terrainGridWidth = heightmap_data->getWidth();
    terrainGridLength = heightmap_data->getHeight();

    terrainHeights.resize(terrainGridWidth * terrainGridLength);
    terrainNormals.resize(terrainGridWidth * terrainGridLength);

    unsigned char* imageData = (unsigned char*)heightmap_data->getData();
    int components = heightmap_data->getBitsPerPixel() / 8;

    for (int i = 0; i < terrainGridLength; i++){
        for (int j = 0; j < terrainGridWidth; j++) {

            float pointHeight = 0;
            if (components == 3) {
                //Convert to greyscale
                pointHeight = 0.2126 * (float)imageData[components * (i * terrainGridWidth + j)] +
                        0.7152 * (float)imageData[components * (i * terrainGridWidth + j) + 1] +
                        0.0722 * (float)imageData[components * (i * terrainGridWidth + j) + 2];
            }else{
                //if RGBA get alpha channel
                pointHeight = imageData[components*(i*terrainGridWidth + j)+(components-1)];
            }

            terrainHeights[i*terrainGridWidth + j] = (pointHeight / 256.0) * 1.0f;
        }
    }

    computeNormals();

    return true;
}

Vector3 Terrain::terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) {

    float *auxNormal,v1[3],v2[3];

    v1[0] = x2-x1;
    v1[1] = -terrainHeights[z1 * terrainGridWidth + x1]
            + terrainHeights[z2 * terrainGridWidth + x2];
    v1[2] = z2-z1;


    v2[0] = x3-x1;
    v2[1] = -terrainHeights[z1 * terrainGridWidth + x1]
            + terrainHeights[z3 * terrainGridWidth + x3];
    v2[2] = z3-z1;

    auxNormal = (float *)malloc(sizeof(float)*3);

    auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
    auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

    return(auxNormal);
}

void Terrain::computeNormals() {
    for (int i = 0; i < terrainGridLength; i++) {
        for (int j = 0; j < terrainGridWidth; j++) {
            Vector3 norm1;
            Vector3 norm2;
            Vector3 norm3;
            Vector3 norm4;

            //For the corners
            if (i == 0 && j == 0) {
                norm1 = terrainCrossProduct(0, 0, 0, 1, 1, 0);
                norm1.normalize();
            } else if (j == terrainGridWidth - 1 && i == terrainGridLength - 1) {
                norm1 = terrainCrossProduct(i, j, j, i - 1, j - 1, i);
                norm1.normalize();
            } else if (j == 0 && i == terrainGridLength - 1) {
                norm1 = terrainCrossProduct(i, j, j, i - 1, j + 1, i);
                norm1.normalize();
            } else if (j == terrainGridWidth - 1 && i == 0) {
                norm1 = terrainCrossProduct(i, j, j, i + 1, j - 1, i);
                norm1.normalize();
            }

            //For the borders
            else if (i == 0) {
                norm1 = terrainCrossProduct(j, 0, j - 1, 0, j, 1);
                norm1.normalize();
                norm2 = terrainCrossProduct(j, 0, j, 1, j + 1, 0);
                norm2.normalize();
            } else if (j == 0) {
                norm1 = terrainCrossProduct(0, i, 1, i, 0, i - 1);
                norm1.normalize();
                norm2 = terrainCrossProduct(0, i, 0, i + 1, 1, i);
                norm2.normalize();
            } else if (i == terrainGridLength) {
                norm1 = terrainCrossProduct(j, i, j, i - 1, j + 1, i);
                norm1.normalize();
                norm2 = terrainCrossProduct(j, i, j + 1, i, j, i - 1);
                norm2.normalize();
            } else if (j == terrainGridWidth) {
                norm1 = terrainCrossProduct(j, i, j, i - 1, j - 1, i);
                norm1.normalize();
                norm2 = terrainCrossProduct(j, i, j - 1, i, j, i + 1);
                norm2.normalize();
            }

            //For the interior
            else {
                norm1 = terrainCrossProduct(j, i, j - 1, i, j, i + 1);
                norm1.normalize();
                norm2 = terrainCrossProduct(j, i, j, i + 1, j + 1, i);
                norm2.normalize();
                norm3 = terrainCrossProduct(j, i, j + 1, i, j, i - 1);
                norm3.normalize();
                norm4 = terrainCrossProduct(j, i, j, i - 1, j - 1, i);
                norm4.normalize();
            }
            if (norm2 != Vector3::ZERO) {
                norm1 += norm2;
            }
            if (norm3 != Vector3::ZERO) {
                norm1 += norm3;
            }
            if (norm4 != Vector3::ZERO) {
                norm1 += norm4;
            }

            norm1.normalize();
            norm1[2] = -norm1[2];
            terrainNormals[i * terrainGridWidth + j] = norm1;
        }
    }
}

bool Terrain::terrainScale(float min, float max) {

    float amp,aux,min1,max1,height;

    if (terrainHeights.size() == 0){
        Log::Error("Terrain is not created");
        return false;
    }

    if (min > max) {
        aux = min;
        min = max;
        max = aux;
    }

    amp = max - min;
    int total = terrainGridWidth * terrainGridLength;

    min1 = terrainHeights[0];
    max1 = terrainHeights[0];
    for(int i=1;i < total ; i++) {
        if (terrainHeights[i] > max1)
            max1 = terrainHeights[i];
        if (terrainHeights[i] < min1)
            min1 = terrainHeights[i];
    }
    for(int i=0;i < total; i++) {
        height = (terrainHeights[i] - min1) / (max1-min1);
        terrainHeights[i] = height * amp - min;
    }

    computeNormals();

    return true;
}

bool Terrain::terrainCreate(float xOffset, float yOffset, float zOffset) {

    float startW = terrainGridWidth / 2.0 - terrainGridWidth;
    float startL = - terrainGridLength / 2.0 + terrainGridLength;

    Attribute* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    Attribute* attTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    Attribute* attNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    for (int i = 0 ; i < terrainGridLength-1; i++) {
        for (int j = 0;j < terrainGridWidth; j++) {

            buffer.addVector3(attVertex,
                    Vector3(startW + j + xOffset,
                            terrainHeights[(i+1)*terrainGridWidth + (j)] + yOffset,
                            startL - (i+1) + zOffset));

            buffer.addVector2(attTexcoord, Vector2((i+1) / (float)terrainGridLength, j / (float)terrainGridWidth));

            buffer.addVector3(attNormal, terrainNormals[(i+1)*terrainGridWidth + j]);

            buffer.addVector3(attVertex,
                              Vector3(startW + j + xOffset,
                                      terrainHeights[(i)*terrainGridWidth + j] + yOffset,
                                      startL - i + zOffset));

            buffer.addVector2(attTexcoord, Vector2(i / (float)terrainGridLength, j / (float)terrainGridWidth));

            buffer.addVector3(attNormal, terrainNormals[i*terrainGridWidth + j]);

        }
    }

    return true;
}

const std::string &Terrain::getHeightmap() const {
    return heightmap_path;
}

void Terrain::setHeightmap(const std::string &heightmap) {
    Terrain::heightmap_path = heightmap;
}

bool Terrain::load(){

    loadTerrainFromImage(heightmap_path);
    //terrainScale(0, 40);
    terrainCreate(0, 0, 0);
/*
    int width = 2000;
    int depth = 2000;

    Attribute* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    buffer.addVector3(attVertex, Vector3(0, 0, 0));
    buffer.addVector3(attVertex, Vector3(0, 0, depth));
    buffer.addVector3(attVertex, Vector3(width, 0, 0));
    buffer.addVector3(attVertex, Vector3(width, 0, depth));

    Attribute* attTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    buffer.addVector2(attTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(attTexcoord, Vector2(0.0f, 1.0f));
    buffer.addVector2(attTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(attTexcoord, Vector2(1.0f, 1.0f));

    Attribute* attNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
*/
    return Mesh::load();
}
