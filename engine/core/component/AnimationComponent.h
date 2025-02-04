//
// (c) 2024 Eduardo Doria.
//

#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "Engine.h"

namespace Supernova{

    struct ActionFrame{
        float startTime = 0;
        float duration = 0;
        Entity action = NULL_ENTITY;
    };

    struct SUPERNOVA_API AnimationComponent{
        std::vector<ActionFrame> actions;
        bool ownedActions = false;
        bool loop = false;

        std::string name;

        float duration = -1; // -1 is infinite
    };

    
}

#endif //ANIMATION_COMPONENT_H