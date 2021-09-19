#include "Polygon.h"

#include "math/Color.h"

using namespace Supernova;

Polygon::Polygon(Scene* scene): Object(scene){
    addComponent<SpriteComponent>({});

    SpriteComponent& spritecomp = getComponent<SpriteComponent>();
    spritecomp.buffer = &buffer;
    spritecomp.indices = &indices;
    spritecomp.primitiveType = PrimitiveType::TRIANGLE_STRIP;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    buffer.addAttribute(AttributeType::COLOR, 4);
}

void Polygon::addVertex(Vector3 vertex){

    buffer.addVector3(AttributeType::POSITION, vertex);
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    if (buffer.getCount() > 3){
        generateTexcoords();
    //    MeshComponent& mesh = getComponent<MeshComponent>();
    //    mesh.primitiveType = S_PRIMITIVE_TRIANGLE_STRIP;
    }
}

void Polygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

void Polygon::generateTexcoords(){

    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        min_X = fmin(min_X, buffer.getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, buffer.getFloat(attVertex, i, 1));
        max_X = fmax(max_X, buffer.getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, buffer.getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        u = (buffer.getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (buffer.getFloat(attVertex, i, 1) - min_Y) * k_Y;
        //if (invertTexture) {
            //buffer.addVector2(AttributeType::TEXTURECOORDS, Vector2(u, 1.0 - v));
       // }else{
            buffer.addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
       // }
    }

    spritecomp.width = (int)(max_X - min_X);
    spritecomp.height = (int)(max_Y - min_Y);
}

void Polygon::setColor(Vector4 color){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.color = Color::sRGBToLinear(color);
}

void Polygon::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Polygon::getColor(){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return Color::linearTosRGB(spritecomp.color);
}

void Polygon::setTexture(std::string path){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.texture.setPath(path);

    //TODO: update texture, reload entity
}

void Polygon::setTextureRect(float x, float y, float width, float height){
    setTextureRect(Rect(x, y, width, height));
}

void Polygon::setTextureRect(Rect textureRect){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.textureRect = textureRect;
}

Rect Polygon::getTextureRect(){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return spritecomp.textureRect;
}