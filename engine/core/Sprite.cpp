#include "Sprite.h"
#include "render/TextureManager.h"


Sprite::Sprite(): Image(){
    sprite = 0;

    texWidth = 0;
    texHeight = 0;

    isSpriteSheet = false;
    spritesX = 1;
    spritesY = 1;
}

Sprite::~Sprite(){

}

void Sprite::updateSpritePosPixels(){
    if (texWidth > 0 && texHeight > 0){
        int spritesPosY;
        int spritesPosX;

        if (sprite < spritesX * spritesY){
            spritesPosY = floor(sprite / spritesX);
            spritesPosX = sprite - spritesX * spritesPosY;
        }else{
            spritesPosY = 0;
            spritesPosX = 0;
        }

        spritePixelsPos = std::make_pair(spritesPosX * texWidth / spritesX, spritesPosY * texHeight / spritesY);
    }
}

void Sprite::setSpriteSheet(int spritesX, int spritesY){
    if (spritesX >= 1 && spritesY >= 1){
        this->spritesX = spritesX;
        this->spritesY = spritesY;

        if ((spritesX > 1 || spritesY > 1) && (material.getTextures().size() > 0)){
            isSpriteSheet = true;
            updateSpritePosPixels();
        }else{
            isSpriteSheet = false;
        }
    }
}

void Sprite::setSprite(int sprite){
    this->sprite = sprite;
    updateSpritePosPixels();
}

bool Sprite::load(){

    if ((material.getTextures().size() > 0) && (isSpriteSheet)) {
        TextureManager::loadTexture(submeshes[0]->getMaterial()->getTextures()[0]);
        texWidth = TextureManager::getTextureWidth(submeshes[0]->getMaterial()->getTextures()[0]);
        texHeight = TextureManager::getTextureHeight(submeshes[0]->getMaterial()->getTextures()[0]);
    }

    renderManager.getRender()->setIsSpriteSheet(isSpriteSheet);
    renderManager.getRender()->setTextureSize(texWidth, texHeight);
    renderManager.getRender()->setSpriteSize(texWidth / spritesX, texHeight / spritesY);
    renderManager.getRender()->setSpritePos(&spritePixelsPos);

    return Image::load();
}
bool Sprite::draw(){
    return Image::draw();
}