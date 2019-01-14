#include "Cube.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

Cube::Cube(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES;

    width = 0;
    height = 0;
    depth = 0;

    buffers["vertices"] = &buffer;

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Cube::Cube(float width, float height, float depth): Cube() {

    this->width = width;
    this->height = height;
    this->depth = depth;

}

Cube::~Cube() {
}

void Cube::createVertices(){

    AttributeData* atrVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);

    buffer.addVector3(atrVertex, Vector3(0, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(width, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(width,  height,  depth));
    buffer.addVector3(atrVertex, Vector3(0, height,  depth));

    buffer.addVector3(atrVertex, Vector3(0, 0, 0));
    buffer.addVector3(atrVertex, Vector3(width, 0, 0));
    buffer.addVector3(atrVertex, Vector3(width,  height, 0));
    buffer.addVector3(atrVertex, Vector3(0,  height, 0));

    buffer.addVector3(atrVertex, Vector3(0, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(0,  height,  depth));
    buffer.addVector3(atrVertex, Vector3(0,  height, 0));
    buffer.addVector3(atrVertex, Vector3(0, 0, 0));

    buffer.addVector3(atrVertex, Vector3(width, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(width,  height,  depth));
    buffer.addVector3(atrVertex, Vector3(width,  height, 0));
    buffer.addVector3(atrVertex, Vector3(width, 0, 0));

    buffer.addVector3(atrVertex, Vector3(0,  height,  depth));
    buffer.addVector3(atrVertex, Vector3(width,  height,  depth));
    buffer.addVector3(atrVertex, Vector3(width,  height, 0));
    buffer.addVector3(atrVertex, Vector3(0,  height, 0));

    buffer.addVector3(atrVertex, Vector3(0, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(width, 0,  depth));
    buffer.addVector3(atrVertex, Vector3(width, 0, 0));
    buffer.addVector3(atrVertex, Vector3(0, 0, 0));

}

void Cube::createTexcoords(){

    AttributeData* atrTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

}

void Cube::createIndices(){

    static const unsigned int indices_array[] = {
            /* front */
            0,  1,  2,
            0,  2,  3,
            /* back */
            4,  5,  6,
            4,  6,  7,
            /* left */
            8,  9, 10,
            8, 10, 11,
            /* right */
            12, 13, 14,
            12, 14, 15,
            /* top */
            16, 17, 18,
            16, 18, 19,
            /* bottom */
            20, 21, 22,
            20, 22, 23,
    };

    std::vector<unsigned int> indices;
    indices.assign(indices_array, std::end(indices_array));
    submeshes[0]->setIndices(indices);

}

void Cube::createNormals(){

    AttributeData* atrNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    
    buffer.addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    
    buffer.addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    
    buffer.addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    
    buffer.addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffer.addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));

}


bool Cube::load(){
    buffer.clear();

    createVertices();
    createTexcoords();
    createIndices();
    createNormals();

    return Mesh::load();
}
