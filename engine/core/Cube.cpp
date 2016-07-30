#include "Cube.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"

#include <stdio.h>

Cube::Cube(): Mesh() {
    primitiveMode = S_TRIANGLES;
}

Cube::Cube(float width, float height, float depth): Cube() {

    vertices.push_back(Vector3(0, 0,  depth));
    vertices.push_back(Vector3(width, 0,  depth));
    vertices.push_back(Vector3(width,  height,  depth));
    vertices.push_back(Vector3(0, height,  depth));

    vertices.push_back(Vector3(0, 0, 0));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(0,  height, 0));

    vertices.push_back(Vector3(0, 0,  depth));
    vertices.push_back(Vector3(0,  height,  depth));
    vertices.push_back(Vector3(0,  height, 0));
    vertices.push_back(Vector3(0, 0, 0));

    vertices.push_back(Vector3(width, 0,  depth));
    vertices.push_back(Vector3(width,  height,  depth));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(width, 0, 0));

    vertices.push_back(Vector3(0,  height,  depth));
    vertices.push_back(Vector3(width,  height,  depth));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(0,  height, 0));

    vertices.push_back(Vector3(0, 0,  depth));
    vertices.push_back(Vector3(width, 0,  depth));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(0, 0, 0));


    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));


    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

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
    submeshes[0].setIndices(indices);

    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));

    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));

    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));

    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

}

Cube::~Cube() {
}
