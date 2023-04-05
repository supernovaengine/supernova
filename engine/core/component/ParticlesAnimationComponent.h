//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

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

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
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

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticleAccelerationInitializer{
        Vector3 minAcceleration = Vector3(0,0,0);
        Vector3 maxAcceleration = Vector3(0,0,0);
    };

    struct ParticleAccelerationModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromAcceleration = Vector3(0,0,0);
        Vector3 toAcceleration = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticleColorInitializer{
        Vector3 minColor = Vector3(1,1,1);
        Vector3 maxColor = Vector3(1,1,1);

        bool useSRGB = true;
    };

    struct ParticleColorModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromColor = Vector3(0,0,0);
        Vector3 toColor = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);

        bool useSRGB = true;
    };

    struct ParticleAlphaInitializer{
        float minAlpha = 1;
        float maxAlpha = 1;
    };

    struct ParticleAlphaModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromAlpha = 0;
        float toAlpha= 0;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticleSizeInitializer{
        float minSize = 0;
        float maxSize = 0;
    };

    struct ParticleSizeModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromSize = 0;
        float toSize = 0;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticleSpriteInitializer{
        std::vector<int> frames;
    };

    struct ParticleSpriteModifier{
        float fromTime = 0;
        float toTime = 0;

        std::vector<int> frames;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
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

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        bool emitter = false;
        bool loop = true;

        int rate = 5; //per second
        int maxPerUpdate = 100;

        int lastUsedParticle = 0;

        ParticleLifeInitializer lifeInitializer;

        ParticlePositionInitializer positionInitializer;
        ParticlePositionModifier positionModifier;

        ParticleVelocityInitializer velocityInitializer;
        ParticleVelocityModifier velocityModifier;

        ParticleAccelerationInitializer accelerationInitializer;
        ParticleAccelerationModifier accelerationModifier;

        ParticleColorInitializer colorInitializer;
        ParticleColorModifier colorModifier;

        ParticleAlphaInitializer alphaInitializer;
        ParticleAlphaModifier alphaModifier;

        ParticleSizeInitializer sizeInitializer;
        ParticleSizeModifier sizeModifier;

        ParticleSpriteInitializer spriteInitializer;
        ParticleSpriteModifier spriteModifier;

        ParticleRotationInitializer rotationInitializer;
        ParticleRotationModifier rotationModifier;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H