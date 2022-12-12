#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    enum class ActionState{
        Running,
        Paused,
        Stopped
    };

    struct ActionComponent{
        Entity target = NULL_ENTITY;
        ActionState state = ActionState::Stopped;
        float timecount = 0;

        float speed = 1;

        bool startTrigger = false;
        bool stopTrigger = false;
        bool pauseTrigger = false;

        FunctionSubscribe<void()> onStart;
        FunctionSubscribe<void()> onPause;
        FunctionSubscribe<void()> onStop;

        FunctionSubscribe<void()> onStep;
    };

    
}

#endif //ACTION_COMPONENT_H