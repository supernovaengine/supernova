//
// (c) 2019 Eduardo Doria.
//

#include "image/TextureLoader.h"
#include "Terrain.h"
#include "Log.h"

using namespace Supernova;

Terrain::Terrain(): Mesh(){

    primitiveType = S_PRIMITIVE_TRIANGLES;
    submeshes.push_back(new Submesh());

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    offset = Vector2(10,10);

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Terrain::Terrain(std::string heightmap): Terrain(){
    this->heightmap_path = heightmap;
}

Terrain::~Terrain(){

}

void Terrain::createPlaneTile(int index, int widthSegments, int heightSegments){
    int width = 5000;
    int height = 5000;

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

    for (int iy = 0; iy < gridY1; iy++) {
        float y = iy * segment_height - height_half;
        for (int ix = 0; ix < gridX1; ix ++) {

            float x = ix * segment_width - width_half;

            buffer.addVector3(attVertex, Vector3(x, 0, -y));
            buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
            buffer.addVector2(attTexcoord, Vector2((float)ix / gridX, (float)iy / gridY));

        }
    }

    unsigned int indexCount = 0;

    for (int iy = 0; iy < gridY; iy++) {
        for (int ix = 0; ix < gridX; ix++) {

            int a = ix + gridX1 * iy;
            int b = ix + gridX1 * ( iy + 1 );
            int c = ( ix + 1 ) + gridX1 * ( iy + 1 );
            int d = ( ix + 1 ) + gridX1 * iy;

            indices.addUInt(attIndice, a);
            indices.addUInt(attIndice, b);
            indices.addUInt(attIndice, d);

            indices.addUInt(attIndice, b);
            indices.addUInt(attIndice, c);
            indices.addUInt(attIndice, d);

            indexCount += 6;

        }
    }

    submeshes[0]->setIndices("indices", indexCount, index * gridX * gridY);

}

bool Terrain::renderLoad(bool shadow){

    if (!shadow){

        instanciateRender();

        render->addProgramDef(S_PROGRAM_IS_TERRAIN);

        render->addProperty(S_PROPERTY_TERRAINTILEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);

    } else {

        instanciateShadowRender();

        //shadowRender->addProgramDef(S_PROGRAM_IS_TERRAIN);

        shadowRender->addProperty(S_PROPERTY_TERRAINTILEOFFSET, S_PROPERTYDATA_FLOAT2, 1, &offset);

    }

    return Mesh::renderLoad(shadow);
};

bool Terrain::load(){

    createPlaneTile(0, 16, 16);
    int oi = 3;
/*
    loadTerrainFromImage(heightmap_path);
    //terrainScale(0, 40);
    terrainCreate(0, 0, 0);

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
