#include "Sprite.h"

#include "math/Color.h"

using namespace Supernova;

Sprite::Sprite(Scene* scene): Polygon(scene){
    addComponent<AnimatedSpriteComponent>({});

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

void Sprite::addFrame(int id, std::string name, Rect rect){
    AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        animsprite.framesRect[id] = {name, rect, true};
    }else{
        Log::Error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Sprite::addFrame(std::string name, float x, float y, float width, float height){
    AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);

    int id = 0;
    while ( (animsprite.framesRect[id].active = true) && (id < MAX_SPRITE_FRAMES) ) {
        id++;
    }

    if (id < MAX_SPRITE_FRAMES){
        addFrame(id, name, Rect(x, y, width, height));
    }else{
        Log::Error("Cannot set frame %s. Sprite frames reached limit of %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Sprite::addFrame(float x, float y, float width, float height){
    addFrame("", x, y, width, height);
}

void Sprite::addFrame(Rect rect){
    addFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Sprite::removeFrame(int id){
    AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);
    animsprite.framesRect[id].active = false;
}

void Sprite::removeFrame(std::string name){
    AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);

    for (int id = 0; id < MAX_SPRITE_FRAMES; id++){
        if (animsprite.framesRect[id].name == name){
            animsprite.framesRect[id].active = false;
        }
    }
}

void Sprite::setFrame(int id){
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);
        setTextureRect(animsprite.framesRect[id].rect);
    }
}

void Sprite::setFrame(std::string name){
    AnimatedSpriteComponent& animsprite = scene->getComponent<AnimatedSpriteComponent>(entity);
    int id = 0;
    while ( (animsprite.framesRect[id].active = false) && (id < MAX_SPRITE_FRAMES) ) {
        id++;
    }

    if (id < MAX_SPRITE_FRAMES){
        setFrame(id);
    }
}
