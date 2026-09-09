// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef Sound_h
#define Sound_h

#include "EntityHandle.h"
#include "Object.h"
#include "component/SoundComponent.h"

namespace doriax {

    class DORIAX_API Sound: public EntityHandle{
    public:
        Sound(Scene* scene);
        Sound(Scene* scene, bool is3D);
        Sound(Scene* scene, Entity entity);
        virtual ~Sound();

        int loadSound(const std::string& filename);
        void destroySound();

        Object getObject() const;

        void play();
        void pause();
        void stop();
        void seek(double time);

        double getLength();
        double getPlayingTime();

        bool isPlaying();
        bool isPaused();
        bool isStopped();

        void setSound3D(bool sound3D);
        bool isSound3D() const;

        void setClockedSound(bool enableClocked);
        bool isClockedSound() const;

        void setVolume(double volume);
        double getVolume() const;

        void setSpeed(float speed);
        float getSpeed() const;

        void setPan(float pan);
        float getPan() const;

        void setLooping(bool looping);
        bool isLooping() const;

        void setLoopingPoint(double loopingPoint);
        double getLoopingPoint() const;

        void setProtectVoice(bool protectVoice);
        bool isProtectVoice() const;

        void setInaudibleBehavior(bool mustTick, bool kill);

        void setMinMaxDistance(float minDistance, float maxDistance);

        void setMinDistance(float minDistance);
        float getMinDistance() const;

        void setMaxDistance(float maxDistance);
        float getMaxDistance() const;

        void setAttenuationModel(SoundAttenuation attenuationModel);
        SoundAttenuation getAttenuationModel() const;

        void setAttenuationRolloffFactor(float attenuationRolloffFactor);
        float getAttenuationRolloffFactor() const;

        void setDopplerFactor(float dopplerFactor);
        float getDopplerFactor() const;
    };

}

#endif /* Sound_h */
