//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

#include "util/Function.h"
#include "action/Ease.h"

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

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleSizeInitializer{
        float minSize = 1;
        float maxSize = 1;
    };

    struct ParticleSizeModifier{
        float fromLife = 0;
        float toLife = 0;

        float fromSize = 0;
        float toSize = 0;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleSpriteInitializer{
        std::vector<int> frames;
    };

    struct ParticleSpriteModifier{
        float fromLife = 0;
        float toLife = 0;

        std::vector<int> frames;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        bool emitter = false;

        int rate = 5; //per second
        int maxPerUpdate = 100;

        int lastUsedParticle = 0;

        ParticleVelocityInitializer velocityInitializer;
        ParticleVelocityModifier velocityModifier;

        ParticleSizeInitializer sizeInitializer;
        ParticleSizeModifier sizeModifier;

        ParticleSpriteInitializer spriteInitializer;
        ParticleSpriteModifier spriteModifier;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H