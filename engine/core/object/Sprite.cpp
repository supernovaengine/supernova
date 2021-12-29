#include "Sprite.h"

#include "math/Color.h"

using namespace Supernova;

Sprite::Sprite(Scene* scene, float width, float height): Mesh(scene){
    animation = NULL;
    addComponent<SpriteComponent>({});

    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.buffers["vertices"] = &buffer;
    mesh.buffers["indices"] = &indices;
    mesh.submeshes[0].hasTextureRect = true;

    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.width = width;
    spritecomp.height = height;

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

    Attribute* attNormal = buffer.getAttribute(AttributeType::NORMAL);

    buffer.addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));

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

Sprite::~Sprite(){
    if (animation)
        delete animation;
}

void Sprite::setBillboard(bool billboard, bool fake, bool cylindrical){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.billboard = billboard;
    spritecomp.fakeBillboard = fake;
    spritecomp.cylindricalBillboard = cylindrical;
}

void Sprite::setTextureRect(float x, float y, float width, float height){
    setTextureRect(Rect(x, y, width, height));
}

void Sprite::setTextureRect(Rect textureRect){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.submeshes[0].textureRect = textureRect;
}

Rect Sprite::getTextureRect(){
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.submeshes[0].textureRect;
}

void Sprite::addFrame(int id, std::string name, Rect rect){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        spritecomp.framesRect[id] = {true, name, rect};
    }else{
        Log::Error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Sprite::addFrame(std::string name, float x, float y, float width, float height){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    int id = 0;
    while ( (spritecomp.framesRect[id].active = true) && (id < MAX_SPRITE_FRAMES) ) {
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
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();
    spritecomp.framesRect[id].active = false;
}

void Sprite::removeFrame(std::string name){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    for (int id = 0; id < MAX_SPRITE_FRAMES; id++){
        if (spritecomp.framesRect[id].name == name){
            spritecomp.framesRect[id].active = false;
        }
    }
}

void Sprite::setFrame(int id){
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        SpriteComponent& spritecomp = getComponent<SpriteComponent>();
        if (spritecomp.framesRect[id].active){
            setTextureRect(spritecomp.framesRect[id].rect);
        }else{
            Log::Error("Cannot use non active sprite frame: %i", id);
        }
    }else{
        Log::Error("Cannot use invalid sprite frame: %i", id);
    }
}

void Sprite::setFrame(std::string name){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();
    int id = 0;
    while ( (!spritecomp.framesRect[id].active) && (id < MAX_SPRITE_FRAMES) ) {
        id++;
    }

    if (id < MAX_SPRITE_FRAMES){
        setFrame(id);
    }else{
        Log::Error("Cannot use nonexistent sprite frame: %s", name.c_str());
    }
}

void Sprite::startAnimation(std::vector<int> frames, std::vector<int> framesTime, bool loop){
    if (!animation){
        animation = new SpriteAnimation(scene);
        animation->setTarget(entity);
    }
    animation->setAnimation(frames, framesTime, loop);
    animation->start();
}

void Sprite::startAnimation(int startFrame, int endFrame, int interval, bool loop){
    if (!animation){
        animation = new SpriteAnimation(scene);
        animation->setTarget(entity);
    }
    animation->setAnimation(startFrame, endFrame, interval, loop);
    animation->start();
}

void Sprite::pauseAnimation(){
    if (animation){
        animation->pause();
    }
}

void Sprite::stopAnimation(){
    if (animation){
        animation->stop();
    }
}
