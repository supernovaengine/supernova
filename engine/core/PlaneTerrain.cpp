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

    vertexBuffer.clearAll();
    vertexBuffer.setName("vertices");
    vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);

    vertexBuffer.clearBuffer();

    AttributeData* attVertex = vertexBuffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    vertexBuffer.addValue(attVertex, Vector3(0, 0, 0));
    vertexBuffer.addValue(attVertex, Vector3(0, 0, depth));
    vertexBuffer.addValue(attVertex, Vector3(width, 0, 0));
    vertexBuffer.addValue(attVertex, Vector3(width, 0, depth));

    AttributeData* attTexcoord = vertexBuffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    vertexBuffer.addValue(attTexcoord, Vector2(0.0f, 0.0f));
    vertexBuffer.addValue(attTexcoord, Vector2(0.0f, 1.0f));
    vertexBuffer.addValue(attTexcoord, Vector2(1.0f, 0.0f));
    vertexBuffer.addValue(attTexcoord, Vector2(1.0f, 1.0f));

    AttributeData* attNormal = vertexBuffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);
    vertexBuffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    vertexBuffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    vertexBuffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    vertexBuffer.addValue(attNormal, Vector3(0.0f, 1.0f, 0.0f));

    return Mesh::load();
}
