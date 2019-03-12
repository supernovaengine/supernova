#include "PlaneTerrain.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

PlaneTerrain::PlaneTerrain(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLE_STRIP;

    buffers["vertices"] = &buffer;

    buffer.clearAll();
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

PlaneTerrain::PlaneTerrain(float width, float depth): PlaneTerrain() {
    this->width = width;
    this->depth = depth;
}

PlaneTerrain::~PlaneTerrain() {
}

bool PlaneTerrain::load(){

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

    return Mesh::load();
}
