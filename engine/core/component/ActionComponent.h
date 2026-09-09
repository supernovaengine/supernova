// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H

#include "Engine.h"

namespace doriax{

    enum class ActionState{
        Running,
        Paused,
        Stopped
    };

    struct DORIAX_API ActionComponent{
        ActionState state = ActionState::Stopped;
        float timecount = 0;

        bool startTrigger = false;
        bool stopTrigger = false;
        bool pauseTrigger = false;

        bool ownedTarget = false;

        Entity target = NULL_ENTITY;

        float speed = 1;

        // Blend weight (0..1) this action contributes when its output is combined
        // with other actions on the same target. Propagated from the owning
        // AnimationComponent during crossfades. Runtime-only (not serialized).
        float weight = 1;

        FunctionSubscribe<void()> onStart;
        FunctionSubscribe<void()> onPause;
        FunctionSubscribe<void()> onStop;

        FunctionSubscribe<void()> onStep;
    };

    
}

#endif //ACTION_COMPONENT_H