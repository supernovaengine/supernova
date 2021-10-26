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
    };
}

#endif //PARTICLESANIMATION_H