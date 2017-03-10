#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"


class Sprite: public Image {
private:
    int texWidth;
    int texHeight;

    std::pair<int, int> spritePixelsPos;

    void updateSpritePosPixels();

protected:
    bool isSpriteSheet;
    int spritesX;
    int spritesY;

    int sprite;

public:
    Sprite();
    virtual ~Sprite();

    void setSprite(int sprite);
    void setSpriteSheet(int spritesX, int spritesY);

    bool load();
    bool draw();
};


#endif //SPRITE_H
