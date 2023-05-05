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
        bool ownedActions = false;
        bool loop = false;

        std::string name;

        float endTime = 0;
    };

    
}

#endif //ANIMATION_COMPONENT_H