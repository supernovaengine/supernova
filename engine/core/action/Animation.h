//
// (c) 2022 Eduardo Doria.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Action.h"

namespace Supernova{
    class Animation: public Action{

    public:
        Animation(Scene* scene);
        Animation(Scene* scene, Entity entity);
        virtual ~Animation();

        Animation(const Animation& rhs);
        Animation& operator=(const Animation& rhs);

        bool isLoop() const;
        void setLoop(bool loop);

        void setEndTime(float endTime);
        float getEndTime() const;

        bool isOwnedActions() const;
        void setOwnedActions(bool ownedActions);

        const std::string &getName() const;
        void setName(const std::string &name);

        void addActionFrame(float startTime, float endTime, Entity action, Entity target);
        void addActionFrame(float startTime, Entity timedaction, Entity target);
        ActionFrame getActionFrame(unsigned int index);
        void clearActionFrames();
    };
}

#endif //ANIMATION_H