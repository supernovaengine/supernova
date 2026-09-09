// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "Engine.h"

namespace doriax{

    struct ActionFrame{
        float startTime = 0;
        float duration = 0; // <= 0 means auto: resolved from the action's own duration
        Entity action = NULL_ENTITY;
        uint32_t track = 0; // Used for editor timeline organization
    };

    struct DORIAX_API AnimationComponent{
        std::vector<ActionFrame> actions;
        bool ownedActions = false;
        bool loop = false;

        float duration = -1; // -1 is infinite

        // Default crossfade time (seconds) used by Model::playAnimation when no
        // explicit fade time is given. Authored/serialized.
        float defaultFadeTime = 0.15f;

        // --- Runtime blend/fade state (not serialized) ---
        // Current blend weight, driven toward fadeTarget at fadeSpeed per second.
        // Applied poses from all animations targeting the same bone are combined
        // by a normalizing weighted average, producing smooth transitions.
        float weight = 1.0f;
        float fadeTarget = 1.0f;
        float fadeSpeed = 0.0f;      // 0 = no fade in progress (instant)
        bool stopOnFadeOut = false;  // stop() the animation once a fade reaches 0
    };

    
}

#endif //ANIMATION_COMPONENT_H
