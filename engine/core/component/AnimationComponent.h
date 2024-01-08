#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "Engine.h"

namespace Supernova{

    struct ActionFrame{
        float startTime;
        float duration;
        Entity action;
    };

    struct AnimationComponent{
        std::vector<ActionFrame> actions;
        bool ownedActions = false;
        bool loop = false;

        std::string name;
    };

    
}

#endif //ANIMATION_COMPONENT_H