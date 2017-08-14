#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include <unordered_map>
#include "action/SpriteAnimation.h"

namespace Supernova {

    class Sprite: public Image {

    protected:
        
        std::unordered_map <std::string, Rect> framesRect;
        SpriteAnimation* defaultAnimation;

    public:
        Sprite();
        virtual ~Sprite();

        void addFrame(std::string id, float x, float y, float width, float height);
        void removeFrame(std::string id);

        void setFrame(std::string id);
        void setFrame(int id);
        
        unsigned int getFramesSize();
        bool isAnimation();

        void playAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop);
        void playAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        void playAnimation(int interval, int startFrame, int endFrame, bool loop);
        void stopAnimation();
        
        virtual bool draw();
    };
    
}


#endif //SPRITE_H
