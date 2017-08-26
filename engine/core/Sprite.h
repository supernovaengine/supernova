#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "action/SpriteAnimation.h"

namespace Supernova {

    class Sprite: public Image {

    protected:

        struct framesData{
            std::string id;
            Rect rect;
        };
        
        std::vector<framesData> framesRect;
        SpriteAnimation* defaultAnimation;

    public:
        Sprite();
        virtual ~Sprite();

        void addFrame(std::string id, float x, float y, float width, float height);
        void addFrame(float x, float y, float width, float height);
        void addFrame(Rect rect);
        void removeFrame(int index);
        void removeFrame(std::string id);

        void setFrame(int index);
        void setFrame(std::string id);

        std::vector<int> findFramesByString(std::string id);
        
        unsigned int getFramesSize();
        bool isAnimation();

        void startAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop);
        void startAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        void startAnimation(int interval, int startFrame, int endFrame, bool loop);
        void startAnimation(int interval, std::vector<int> frames, bool loop);
        void stopAnimation();
        
        virtual bool draw();
    };
    
}


#endif //SPRITE_H
