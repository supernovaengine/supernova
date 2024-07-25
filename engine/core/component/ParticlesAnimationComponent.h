//
// (c) 2024 Eduardo Doria.
//

#ifndef PARTICLESANIMATION_COMPONENT_H
#define PARTICLESANIMATION_COMPONENT_H

#include "action/Ease.h"

namespace Supernova{

    struct PointParticleLifeInitializer{
        float minLife = 10;
        float maxLife = 10;
    };

    struct PointParticlePositionInitializer{
        Vector3 minPosition = Vector3(0,0,0);
        Vector3 maxPosition = Vector3(0,0,0);
    };

    struct PointParticlePositionModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromPosition = Vector3(0,0,0);
        Vector3 toPosition = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleVelocityInitializer{
        Vector3 minVelocity = Vector3(0,0,0);
        Vector3 maxVelocity = Vector3(0,0,0);
    };

    struct PointParticleVelocityModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromVelocity = Vector3(0,0,0);
        Vector3 toVelocity = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleAccelerationInitializer{
        Vector3 minAcceleration = Vector3(0,0,0);
        Vector3 maxAcceleration = Vector3(0,0,0);
    };

    struct PointParticleAccelerationModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromAcceleration = Vector3(0,0,0);
        Vector3 toAcceleration = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleColorInitializer{
        Vector3 minColor = Vector3(1,1,1);
        Vector3 maxColor = Vector3(1,1,1);

        bool useSRGB = true;
    };

    struct PointParticleColorModifier{
        float fromTime = 0;
        float toTime = 0;

        Vector3 fromColor = Vector3(0,0,0);
        Vector3 toColor = Vector3(0,0,0);

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);

        bool useSRGB = true;
    };

    struct PointParticleAlphaInitializer{
        float minAlpha = 1;
        float maxAlpha = 1;
    };

    struct PointParticleAlphaModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromAlpha = 0;
        float toAlpha= 0;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleSizeInitializer{
        float minSize = 0;
        float maxSize = 0;
    };

    struct PointParticleSizeModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromSize = 0;
        float toSize = 0;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleSpriteInitializer{
        std::vector<int> frames;
    };

    struct PointParticleSpriteModifier{
        float fromTime = 0;
        float toTime = 0;

        std::vector<int> frames;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct PointParticleRotationInitializer{
        float minRotation = 0;
        float maxRotation = 0;
    };

    struct PointParticleRotationModifier{
        float fromTime = 0;
        float toTime = 0;

        float fromRotation = 0;
        float toRotation = 0;

        FunctionSubscribe<float(float)> function = std::function<float(float)>(Ease::linear);
    };

    struct ParticlesAnimationComponent{
        float newParticlesCount = 0;
        int lastUsedParticle = 0;
        bool emitter = false;

        bool loop = true;

        int rate = 5; //per second
        int maxPerUpdate = 100;

        PointParticleLifeInitializer lifeInitializer;

        PointParticlePositionInitializer positionInitializer;
        PointParticlePositionModifier positionModifier;

        PointParticleVelocityInitializer velocityInitializer;
        PointParticleVelocityModifier velocityModifier;

        PointParticleAccelerationInitializer accelerationInitializer;
        PointParticleAccelerationModifier accelerationModifier;

        PointParticleColorInitializer colorInitializer;
        PointParticleColorModifier colorModifier;

        PointParticleAlphaInitializer alphaInitializer;
        PointParticleAlphaModifier alphaModifier;

        PointParticleSizeInitializer sizeInitializer;
        PointParticleSizeModifier sizeModifier;

        PointParticleSpriteInitializer spriteInitializer;
        PointParticleSpriteModifier spriteModifier;

        PointParticleRotationInitializer rotationInitializer;
        PointParticleRotationModifier rotationModifier;
    };

}

#endif //PARTICLESANIMATION_COMPONENT_H