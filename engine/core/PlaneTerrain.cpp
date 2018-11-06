#include "PlaneTerrain.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

PlaneTerrain::PlaneTerrain(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES_STRIP;
}

PlaneTerrain::PlaneTerrain(float width, float depth): PlaneTerrain() {
    this->width = width;
    this->depth = depth;
}

PlaneTerrain::~PlaneTerrain() {
}

bool PlaneTerrain::load(){

    buffer.clearAll();
    buffer.setName("vertices");
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);

    buffer.clearBuffer();

    AttributeData* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    buffer.addValue(attVertex, Vector3(0, 0, 0));
    buffer.addValue(attVertex, Vector3(0, 0, depth));
    buffer.addValue(attVertex, Vector3(width, 0, 0));
    buffer.addValue(attVertex, Vector3(width, 0, depth));

    AttributeData* attTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    buffer.addValue(attTexcoord, Vector2(0.0f, 0.0f));
    buffer.addValue(attTexcoord, Vector2(0.0f, 1.0f));
    buffer.addValue(attTexcoord, Vector2(1.0f, 0.0f));
    buffer.addValue(attTexcoord, Vector2(1.0f, 1.0f));

    AttributeData* attNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);
    buffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));

    return Mesh::load();
}
