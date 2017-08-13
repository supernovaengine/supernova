#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include <unordered_map>

namespace Supernova {

    class Sprite: public Image {
        
    private:
        
        bool inAnimation;

    protected:
        
        struct animationData{
            std::vector<int> frames;
            std::vector<int> framesTime;
            int framesIndex;
            int framesTimeIndex;
            bool loop;
            int timecount;
        };
        
        std::unordered_map <std::string, Rect> framesRect;
        
        animationData animation;
        
    public:
        Sprite();
        virtual ~Sprite();

        void addFrame(std::string id, float x, float y, float width, float height);
        void removeFrame(std::string id);

        void setFrame(std::string id);
        void setFrame(int id);

        void animate(std::vector<int> framesTime, std::vector<int> frames, bool loop);
        void animate(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        void animate(int interval, int startFrame, int endFrame, bool loop);
        
        virtual bool draw();
    };
    
}


#endif //SPRITE_H
