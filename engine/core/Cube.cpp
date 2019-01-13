#include "Cube.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

Cube::Cube(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES;

    width = 0;
    height = 0;
    depth = 0;

    buffers.push_back(new InterleavedBuffer());

    buffers[0]->clearAll();
    buffers[0]->setName("vertices");
    ((InterleavedBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    ((InterleavedBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    ((InterleavedBuffer*)buffers[0])->addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Cube::Cube(float width, float height, float depth): Cube() {

    this->width = width;
    this->height = height;
    this->depth = depth;

}

Cube::~Cube() {
}

void Cube::createVertices(){

    AttributeData* atrVertex = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_VERTICES);

    buffers[0]->addVector3(atrVertex, Vector3(0, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(0, height,  depth));

    buffers[0]->addVector3(atrVertex, Vector3(0, 0, 0));
    buffers[0]->addVector3(atrVertex, Vector3(width, 0, 0));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height, 0));
    buffers[0]->addVector3(atrVertex, Vector3(0,  height, 0));

    buffers[0]->addVector3(atrVertex, Vector3(0, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(0,  height,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(0,  height, 0));
    buffers[0]->addVector3(atrVertex, Vector3(0, 0, 0));

    buffers[0]->addVector3(atrVertex, Vector3(width, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height, 0));
    buffers[0]->addVector3(atrVertex, Vector3(width, 0, 0));

    buffers[0]->addVector3(atrVertex, Vector3(0,  height,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width,  height, 0));
    buffers[0]->addVector3(atrVertex, Vector3(0,  height, 0));

    buffers[0]->addVector3(atrVertex, Vector3(0, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width, 0,  depth));
    buffers[0]->addVector3(atrVertex, Vector3(width, 0, 0));
    buffers[0]->addVector3(atrVertex, Vector3(0, 0, 0));

}

void Cube::createTexcoords(){

    AttributeData* atrTexcoord = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    buffers[0]->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

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

    AttributeData* atrNormal = buffers[0]->getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 0.0f, -1.0f));
    
    buffers[0]->addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(-1.0f, 0.0f, 0.0f));
    
    buffers[0]->addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(1.0f, 0.0f, 0.0f));
    
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, 1.0f, 0.0f));
    
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));
    buffers[0]->addVector3(atrNormal, Vector3(0.0f, -1.0f, 0.0f));

}


bool Cube::load(){
    buffers[0]->clear();

    createVertices();
    createTexcoords();
    createIndices();
    createNormals();

    return Mesh::load();
}
