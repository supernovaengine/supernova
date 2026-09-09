// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Action.h"
#include "component/ParticlesComponent.h"

namespace doriax{

    class DORIAX_API Particles: public Action{
    public:
        Particles(Scene* scene);
        Particles(Scene* scene, Entity entity);
        virtual ~Particles();

        void reset();

        void setMaxParticles(unsigned int maxParticles);
        unsigned int getMaxParticles() const;

        void setRate(int rate);
        int getRate() const;

        void setEmitter(bool emitter);
        bool isEmitter() const;

        void setLoop(bool loop);
        bool isLoop() const;

        void setLocalSpace(bool localSpace);
        bool isLocalSpace() const;

        void setMaxPerUpdate(int maxPerUpdate);
        int getMaxPerUpdate() const;

        void setLifeInitializer(float life);
        void setLifeInitializer(float minLife, float maxLife);

        void setPositionInitializer(Vector3 position);
        void setPositionInitializer(Vector3 minPosition, Vector3 maxPosition);
        void setPositionInitializerShape(ParticleEmitterShape shape);
        ParticleEmitterShape getPositionInitializerShape() const;
        void setSpherePositionInitializer(float radius);
        void setSpherePositionInitializer(float radius, float innerRadius);
        void setHemispherePositionInitializer(float radius);
        void setHemispherePositionInitializer(float radius, float innerRadius);
        void setCirclePositionInitializer(float radius);
        void setCirclePositionInitializer(float radius, float innerRadius);
        void setConePositionInitializer(float angle, float height);
        void setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition);
        void setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition, EaseType functionType);
        void setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition, Ease function);

        void addBurst(float time, int count);
        void addBurst(float time, int minCount, int maxCount);
        void setBursts(std::vector<ParticleBurst> bursts);
        void clearBursts();

        void setVelocityInitializer(Vector3 velocity);
        void setVelocityInitializer(Vector3 minVelocity, Vector3 maxVelocity);
        void setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity);
        void setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity, EaseType functionType);
        void setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity, Ease function);

        void setAccelerationInitializer(Vector3 acceleration);
        void setAccelerationInitializer(Vector3 minAcceleration, Vector3 maxAcceleration);
        void setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration);
        void setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration, EaseType functionType);
        void setAccelerationModifier(float fromTime, float toTime, Vector3 fromAcceleration, Vector3 toAcceleration, Ease function);

        void setColorInitializer(Vector3 color);
        void setColorInitializer(Vector3 minColor, Vector3 maxColor);
        void setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor);
        void setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor, EaseType functionType);
        void setColorModifier(float fromTime, float toTime, Vector3 fromColor, Vector3 toColor, Ease function);

        void addColorGradientStop(float time, Vector3 color);
        void setColorGradient(std::vector<ParticleColorGradientStop> stops);
        void clearColorGradient();
        void setColorGradientUseSRGB(bool useSRGB);

        void setAlphaInitializer(float alpha);
        void setAlphaInitializer(float minAlpha, float maxAlpha);
        void setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha);
        void setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha, EaseType functionType);
        void setAlphaModifier(float fromTime, float toTime, float fromAlpha, float toAlpha, Ease function);

        void setSizeInitializer(float size);
        void setSizeInitializer(float minSize, float maxSize);
        void setSizeModifier(float fromTime, float toTime, float fromSize, float toSize);
        void setSizeModifier(float fromTime, float toTime, float fromSize, float toSize, EaseType functionType);
        void setSizeModifier(float fromTime, float toTime, float fromSize, float toSize, Ease function);

        void setSpriteIntializer(std::vector<int> frames);
        void setSpriteIntializer(int minFrame, int maxFrame);
        void setSpriteModifier(float fromTime, float toTime, std::vector<int> frames);
        void setSpriteModifier(float fromTime, float toTime, std::vector<int> frames, EaseType functionType);
        void setSpriteModifier(float fromTime, float toTime, std::vector<int> frames, Ease function);

        void setRotationInitializer(Quaternion rotation);
        void setRotationInitializer(float rotation);
        void setRotationInitializer(Quaternion minRotation, Quaternion maxRotation);
        void setRotationInitializer(float minRotation, float maxRotation);
        void setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation);
        void setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation);
        void setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation, EaseType functionType);
        void setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation, EaseType functionType);
        void setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation, Ease function);
        void setRotationModifier(float fromTime, float toTime, Quaternion fromRotation, Quaternion toRotation, Ease function);

        void setScaleInitializer(float scale);
        void setScaleInitializer(Vector3 scale);
        void setScaleInitializer(float minScale, float maxScale);
        void setScaleInitializer(Vector3 minScale, Vector3 maxScale);
        void setScaleModifier(float fromTime, float toTime, float fromScale, float toScale);
        void setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale);
        void setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale, EaseType functionType);
        void setScaleModifier(float fromTime, float toTime, Vector3 fromScale, Vector3 toScale, Ease function);
    };
}

#endif //PARTICLES_H