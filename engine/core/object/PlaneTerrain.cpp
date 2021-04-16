#include "PlaneTerrain.h"

using namespace Supernova;


PlaneTerrain::PlaneTerrain(Scene* scene): Mesh(scene){
    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);
    mesh.buffers["vertices"] = &buffer;
    mesh.buffers["indices"] = &indices;

    mesh.submeshes[1].primitiveType = PrimitiveType::TRIANGLES;
    mesh.numSubmeshes = 1;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITIONS, 3);
	buffer.addAttribute(AttributeType::TEXTURECOORDS, 2);
	buffer.addAttribute(AttributeType::NORMALS, 3);
    buffer.addAttribute(AttributeType::COLORS, 4);
}

PlaneTerrain::PlaneTerrain(Scene* scene, float width, float depth): PlaneTerrain(scene){
    create(width, depth);
}

void PlaneTerrain::create(float width, float depth){
    buffer.clear();
    indices.clear();

    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITIONS);
    buffer.addVector3(attVertex, Vector3(0, 0, 0));
    buffer.addVector3(attVertex, Vector3(0, 0, depth));
    buffer.addVector3(attVertex, Vector3(width, 0, depth));
    buffer.addVector3(attVertex, Vector3(width, 0, 0));

    Attribute* attTexcoord = buffer.getAttribute(AttributeType::TEXTURECOORDS);
    buffer.addVector2(attTexcoord, Vector2(0.0f, 0.0f));
    buffer.addVector2(attTexcoord, Vector2(0.0f, 1.0f));
    buffer.addVector2(attTexcoord, Vector2(1.0f, 1.0f));
    buffer.addVector2(attTexcoord, Vector2(1.0f, 0.0f));

    Attribute* attNormal = buffer.getAttribute(AttributeType::NORMALS);
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));

    Attribute* attColor = buffer.getAttribute(AttributeType::COLORS);
    buffer.addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer.addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer.addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer.addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    static const uint16_t indices_array[] = {
        0,  1,  2,
        0,  2,  3,
    };

    indices.setValues(
        0, indices.getAttribute(AttributeType::INDEX),
        6, (char*)&indices_array[0], sizeof(uint16_t));

    addSubmeshAttribute(mesh.submeshes[1], "indices", AttributeType::INDEX, 1, AttributeDataType::UNSIGNED_SHORT, 6, indices.getCount() * sizeof(uint16_t), false);
}