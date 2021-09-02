#include "Sprite.h"

#include "math/Color.h"

using namespace Supernova;

Sprite::Sprite(Scene* scene): Polygon(scene){

    SpriteComponent& spritecomp = scene->getComponent<SpriteComponent>(entity);
    spritecomp.primitiveType = PrimitiveType::TRIANGLES;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    buffer.addAttribute(AttributeType::COLOR, 4);

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);

    buffer.addVector3(attVertex, Vector3(0, 0, 0));
    buffer.addVector3(attVertex, Vector3(spritecomp.width, 0, 0));
    buffer.addVector3(attVertex, Vector3(spritecomp.width,  spritecomp.height, 0));
    buffer.addVector3(attVertex, Vector3(0,  spritecomp.height, 0));

    Attribute* attTexcoord = buffer.getAttribute(AttributeType::TEXCOORD1);

    buffer.addVector2(attTexcoord, Vector2(0.01f, 0.01f));
    buffer.addVector2(attTexcoord, Vector2(0.99f, 0.01f));
    buffer.addVector2(attTexcoord, Vector2(0.99f, 0.99f));
    buffer.addVector2(attTexcoord, Vector2(0.01f, 0.99f));

    Attribute* attColor = buffer.getAttribute(AttributeType::COLOR);

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
}