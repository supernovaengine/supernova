// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Action.h"

namespace doriax{
    class DORIAX_API Animation: public Action{

    public:
        Animation(Scene* scene);
        Animation(Scene* scene, Entity entity);
        virtual ~Animation();

        Animation(const Animation& rhs);
        Animation& operator=(const Animation& rhs);

        bool isLoop() const;
        void setLoop(bool loop);

        // Crossfade helpers. fadeIn starts a stopped animation at weight 0 or
        // reverses a running fade from its current weight, then ramps it to 1.
        // fadeOut ramps a running animation to 0 and stops it. A duration <= 0
        // is instant.
        void fadeIn(float duration);
        void fadeOut(float duration);

        using Action::start; // keep base start() visible alongside the fade overload
        void start(float fadeInDuration);

        float getBlendWeight() const;
        void setBlendWeight(float weight);

        float getDefaultFadeTime() const;
        void setDefaultFadeTime(float time);

        bool isOwnedActions() const;
        void setOwnedActions(bool ownedActions);

        const float &getDuration() const;
        void setDuration(const float &duration);

        void addActionFrame(float startTime, float duration, Entity action, Entity target);
        void addActionFrame(float startTime, Entity timedaction, Entity target);
        void addActionFrame(float startTime, float duration, Entity action);
        void addActionFrame(float startTime, Entity timedaction);

        size_t getActionFrameSize() const;
        ActionFrame& getActionFrame(unsigned int index);
        void setActionFrameStartTime(unsigned int index, float startTime);
        void setActionFrameDuration(unsigned int index, float duration);
        void setActionFrameEntity(unsigned int index, Entity action);

        void clearActionFrames();
    };
}

#endif //ANIMATION_H
