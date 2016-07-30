#include "Plane.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"

Plane::Plane(): Mesh() {
    primitiveMode = S_TRIANGLES_STRIP;
}

Plane::Plane(float width, float depth): Plane() {
    vertices.push_back(Vector3(0, 0, 0));
    vertices.push_back(Vector3(0, 0, depth));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(width, 0, depth));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));

    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
}

Plane::~Plane() {
}
