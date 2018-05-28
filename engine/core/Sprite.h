#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "action/SpriteAnimation.h"

namespace Supernova {

    class Sprite: public Image {

    protected:

        struct frameData{
            std::string id;
            Rect rect;
        };
        
        std::vector<frameData> framesRect;
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

        void runAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop);
        void runAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        void runAnimation(int interval, int startFrame, int endFrame, bool loop);
        void runAnimation(int interval, std::vector<int> frames, bool loop);
        void stopAnimation();

        virtual bool load();
        virtual bool draw();
    };
    
}


#endif //SPRITE_H
