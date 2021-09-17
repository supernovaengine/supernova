//
// (c) 2021 Eduardo Doria.
//

#ifndef SPRITE_H
#define SPRITE_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"
#include "action/SpriteAnimation.h"

#include "Polygon.h"

namespace Supernova{
    class Sprite: public Polygon{

    protected:
        SpriteAnimation* animation;

    public:
        Sprite(Scene* scene);
        virtual ~Sprite();

        void addFrame(int id, std::string name, Rect rect);
        void addFrame(std::string name, float x, float y, float width, float height);
        void addFrame(float x, float y, float width, float height);
        void addFrame(Rect rect);
        void removeFrame(int id);
        void removeFrame(std::string name);

        void setFrame(int id);
        void setFrame(std::string name);

        void startAnimation(std::vector<int> frames, std::vector<int> framesTime, bool loop);
        void startAnimation(int startFrame, int endFrame, int interval, bool loop);
        void stopAnimation();
    };
}

#endif //SPRITE_H