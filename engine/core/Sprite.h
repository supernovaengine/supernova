#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include "action/SpriteAnimation.h"
#include <map>

namespace Supernova {

    class Sprite: public Image {

    protected:

        struct frameData{
            std::string name;
            Rect rect;
        };
        
        std::map<int,frameData> framesRect;
        SpriteAnimation* defaultAnimation;

    public:
        Sprite();
        virtual ~Sprite();

        void addFrame(int id, std::string name, Rect rect);
        void addFrame(std::string name, float x, float y, float width, float height);
        void addFrame(float x, float y, float width, float height);
        void addFrame(Rect rect);
        void removeFrame(int id);
        void removeFrame(std::string name);

        void setFrame(int id);
        void setFrame(std::string name);

        std::vector<int> findFramesByString(std::string name);
        
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
