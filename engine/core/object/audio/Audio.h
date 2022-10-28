

#ifndef Audio_h
#define Audio_h

#include "Object.h"
#include "component/AudioComponent.h"

namespace Supernova {

    class Audio: public Object{
    public:
        Audio(Scene* scene);
        virtual ~Audio();

        int loadAudio(std::string filename);
        void destroyAudio();

        void play();
        void pause();
        void stop();
        void seek(double time);

        double getLength();
        double getPlayingTime();

        bool isPlaying();
        bool isPaused();
        bool isStopped();

        void set3DSound(bool enable3D);
        bool is3DSound() const;

        void setClockedSound(bool enableClocked);
        bool isClockedSound() const;

        void setVolume(double volume);
        double getVolume() const;

        void setSpeed(float speed);
        float getSpeed() const;

        void setPan(float pan);
        float getPan() const;

        void setLopping(bool lopping);
        bool isLopping() const;

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

        void setAttenuationModel(AudioAttenuation attenuationModel);
        AudioAttenuation getAttenuationModel() const;

        void setAttenuationRolloffFactor(float attenuationRolloffFactor);
        float getAttenuationRolloffFactor() const;

        void setDopplerFactor(float dopplerFactor);
        float getDopplerFactor() const;
    };

}

#endif /* Audio_h */
