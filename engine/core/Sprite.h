#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include <unordered_map>

namespace Supernova {

    class Sprite: public Image {

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
    
}


#endif //SPRITE_H
