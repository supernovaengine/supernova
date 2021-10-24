//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

namespace Supernova{

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        bool emitter = false;

        int minRate = 5; //per second
        int maxRate = 10;

        int lastUsedParticle = 0;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H