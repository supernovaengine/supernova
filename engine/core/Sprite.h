#ifndef SPRITE_H
#define SPRITE_H

#include "RectImage.h"
#include <unordered_map>


class Sprite: public RectImage {

protected:
    std::unordered_map <std::string, Rect> framesRect;

public:
    Sprite();
    virtual ~Sprite();

    void addFrame(std::string id, float x, float y, float width, float height);
    void removeFrame(std::string id);

    void setFrame(std::string id);
    void setFrame(int id);
};


#endif //SPRITE_H
