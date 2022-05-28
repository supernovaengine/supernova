#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    struct ActionFrame{
        float startTime;
        float endTime;
        Entity action;
    };

    struct AnimationComponent{
        std::vector<ActionFrame> actions;
        bool ownedActions;
        bool loop;

        std::string name;

        float startTime;
        float endTime;
    };

    
}

#endif //ANIMATION_COMPONENT_H