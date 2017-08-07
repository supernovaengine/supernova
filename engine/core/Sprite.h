#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include <unordered_map>

namespace Supernova {

    class Sprite: public Image {
        
    private:
        
        bool inAnimation;
        int animationFrame;
        unsigned int animationAcc;

    protected:
        
        struct animationData{
            std::vector<int> framesTime;
            int startFrame;
            int endFrame;
            bool loop;
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
        
        void animate(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        
        virtual bool draw();
    };
    
}


#endif //SPRITE_H
