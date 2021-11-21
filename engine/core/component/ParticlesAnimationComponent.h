//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

#include "util/Function.h"
#include "action/Ease.h"

namespace Supernova{

    struct ParticleLifeInitializer{
        float minLife = 10;
        float maxLife = 10;
    };

    struct ParticlePositionInitializer{
        Vector3 minPosition = Vector3(0,0,0);
        Vector3 maxPosition = Vector3(0,0,0);
    };

    struct ParticlePositionModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromPosition = Vector3(0,0,0);
        Vector3 toPosition = Vector3(0,0,0);

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleVelocityInitializer{
        Vector3 minVelocity = Vector3(0,0,0);
        Vector3 maxVelocity = Vector3(0,0,0);
    };

    struct ParticleVelocityModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromVelocity = Vector3(0,0,0);
        Vector3 toVelocity = Vector3(0,0,0);

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleSizeInitializer{
        float minSize = 1;
        float maxSize = 1;
    };

    struct ParticleSizeModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromSize = 0;
        float toSize = 0;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleSpriteInitializer{
        std::vector<int> frames;
    };

    struct ParticleSpriteModifier{
        float fromTime = 0;
        float toTime = 0;

        std::vector<int> frames;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticleRotationInitializer{
        float minRotation = 0;
        float maxRotation = 0;
    };

    struct ParticleRotationModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromRotation = 0;
        float toRotation = 0;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        bool emitter = false;

        int rate = 5; //per second
        int maxPerUpdate = 100;

        int lastUsedParticle = 0;

        ParticleLifeInitializer lifeInitializer;

        ParticlePositionInitializer positionInitializer;
        ParticlePositionModifier positionModifier;

        ParticleVelocityInitializer velocityInitializer;
        ParticleVelocityModifier velocityModifier;

        ParticleSizeInitializer sizeInitializer;
        ParticleSizeModifier sizeModifier;

        ParticleSpriteInitializer spriteInitializer;
        ParticleSpriteModifier spriteModifier;

        ParticleRotationInitializer rotationInitializer;
        ParticleRotationModifier rotationModifier;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H