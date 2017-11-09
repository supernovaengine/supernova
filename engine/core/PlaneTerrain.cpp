#include "PlaneTerrain.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"

using namespace Supernova;

PlaneTerrain::PlaneTerrain(): Mesh() {
    primitiveMode = S_TRIANGLES_STRIP;
}

PlaneTerrain::PlaneTerrain(float width, float depth): PlaneTerrain() {
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

PlaneTerrain::~PlaneTerrain() {
}
