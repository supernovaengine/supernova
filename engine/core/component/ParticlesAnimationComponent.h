//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

namespace Supernova{

    struct ParticleVelocityInitializer{
        Vector3 minVelocity = Vector3(0,0,0);
        Vector3 maxVelocity = Vector3(0,0,0);
    };

    struct ParticleVelocityModifier{
        float fromLife = 0;
        float toLife = 0;

        Vector3 fromVelocity = Vector3(0,0,0);
        Vector3 toVelocity = Vector3(0,0,0);
    };

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        bool emitter = false;

        int rate = 5; //per second
        int maxPerUpdate = 100;

        int lastUsedParticle = 0;

        ParticleVelocityInitializer velocityInitializer;
        ParticleVelocityModifier velocityModifier;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H