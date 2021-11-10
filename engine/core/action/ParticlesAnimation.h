//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_H
#define PARTICLESANIMATION_H

#include "Action.h"

namespace Supernova{
    class ParticlesAnimation: public Action{

    public:
        ParticlesAnimation(Scene* scene);

        void setVelocityInitializer(Vector3 velocity);
        void setVelocityInitializer(Vector3 minVelocity, Vector3 maxVelocity);
        void setVelocityModifier(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity);
        void setVelocityModifier(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity, int functionType);

        void setSizeInitializer(float size);
        void setSizeInitializer(float minSize, float maxSize);
        void setSizeModifier(float fromLife, float toLife, float fromSize, float toSize);
        void setSizeModifier(float fromLife, float toLife, float fromSize, float toSize, int functionType);

        void setSpriteIntializer(std::vector<int> frames);
        void setSpriteIntializer(int minFrame, int maxFrame);
        void setSpriteModifier(float fromLife, float toLife, std::vector<int> frames);
        void setSpriteModifier(float fromLife, float toLife, std::vector<int> frames, int functionType);

        void setRotationInitializer(float rotation);
        void setRotationInitializer(float minRotation, float maxRotation);
        void setRotationModifier(float fromLife, float toLife, float fromRotation, float toRotation);
        void setRotationModifier(float fromLife, float toLife, float fromRotation, float toRotation, int functionType);
    };
}

#endif //PARTICLESANIMATION_H