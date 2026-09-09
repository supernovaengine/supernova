// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SOUND_COMPONENT_H
#define SOUND_COMPONENT_H

#include "Engine.h"

#include <memory>

namespace SoLoud{
    class Wav;
}

namespace doriax{

    enum class SoundState{
        Playing,
        Paused,
        Stopped
    };

    enum class SoundAttenuation{
        NO_ATTENUATION,
        INVERSE_DISTANCE,
        LINEAR_DISTANCE,
        EXPONENTIAL_DISTANCE
    };

    struct DORIAX_API SoundComponent{
        std::shared_ptr<SoLoud::Wav> sample = nullptr;
        unsigned int handle = 0; //Soloud handle

        SoundState state = SoundState::Stopped;

        std::string filename;
        bool loaded = false;

        bool enableClocked = false;
        Vector3 lastPosition = Vector3(0, 0, 0);

        bool startTrigger = false;
        bool stopTrigger = false;
        bool pauseTrigger = false;

        FunctionSubscribe<void()> onStart;
        FunctionSubscribe<void()> onPause;
        FunctionSubscribe<void()> onStop;

        double volume = 1;
        float speed = 1;
        float pan = 0; // -1 is left, 0 is middle and 1 is right
        bool looping = false;
        double loopingPoint = 0;
        bool protectVoice = false;
        bool inaudibleBehaviorMustTick = false;
        bool inaudibleBehaviorKill = false;

        // for 3D sound
        float minDistance = 1.0;
        float maxDistance = 1000.0;
        SoundAttenuation attenuationModel = SoundAttenuation::NO_ATTENUATION;
        float attenuationRolloffFactor = 1.0; // means that end of max distance sound will be zero
        float dopplerFactor = 1.0;

        // setted by system
        double length = 0;
        double playingTime = 0;

        bool needUpdate = true;
    };

}

#endif //SOUND_COMPONENT_H
