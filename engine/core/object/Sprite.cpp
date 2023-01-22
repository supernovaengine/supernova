//
// (c) 2023 Eduardo Doria.
//

#include "Sprite.h"

#include "util/Color.h"

using namespace Supernova;

Sprite::Sprite(Scene* scene): Mesh(scene){
    addComponent<SpriteComponent>({});
    animation = NULL;
}

Sprite::~Sprite(){
    if (animation)
        delete animation;
}

Sprite::Sprite(const Sprite& rhs): Mesh(rhs){
    animation = rhs.animation;
}

Sprite& Sprite::operator=(const Sprite& rhs){
    animation = rhs.animation;
    Mesh::operator=(rhs);

    return *this;
}

void Sprite::setSize(int width, int height){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    if ((spritecomp.width != width) || (spritecomp.height != height)){
        spritecomp.width = width;
        spritecomp.height = height;

        spritecomp.needUpdateSprite = true;
    }
}

void Sprite::setWidth(int width){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    if (spritecomp.width != width){
        spritecomp.width = width;

        spritecomp.needUpdateSprite = true;
    }
}

void Sprite::setHeight(int height){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    if (spritecomp.height != height){
        spritecomp.height = height;

        spritecomp.needUpdateSprite = true;
    }
}

int Sprite::getWidth() const{
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return spritecomp.width;
}

int Sprite::getHeight() const{
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return spritecomp.height;
}

void Sprite::setFlipY(bool flipY){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.automaticFlipY = false;
    if (spritecomp.flipY != flipY){
        spritecomp.flipY = flipY;

        spritecomp.needUpdateSprite = true;
    }
}

bool Sprite::isFlipY() const{
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return spritecomp.flipY;
}

void Sprite::setTextureRect(float x, float y, float width, float height){
    setTextureRect(Rect(x, y, width, height));
}

void Sprite::setTextureRect(Rect textureRect){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.submeshes[0].textureRect = textureRect;
}

Rect Sprite::getTextureRect() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.submeshes[0].textureRect;
}

void Sprite::setPivot(PivotPreset pivot){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    spritecomp.pivot = pivot;
    spritecomp.needUpdateSprite = true;
}

PivotPreset Sprite::getPivot() const{
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();

    return spritecomp.pivot;
}

void Sprite::addFrame(int id, std::string name, Rect rect){
    SpriteComponent& spritecomp = getComponent<SpriteComponent>();
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        spritecomp.framesRect[id] = {true, name, rect};
    }else{
        Log::error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
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
        Log::error("Cannot set frame %s. Sprite frames reached limit of %i", name.c_str(), MAX_SPRITE_FRAMES);
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
            Log::error("Cannot use non active sprite frame: %i", id);
        }
    }else{
        Log::error("Cannot use invalid sprite frame: %i", id);
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
        Log::error("Cannot use nonexistent sprite frame: %s", name.c_str());
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
