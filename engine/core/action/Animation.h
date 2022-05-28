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

        bool isLoop();
        void setLoop(bool loop);

        void setStartFrame(int frameIndex);
        void setStartTime(float startTime);
        float getStartTime();
/*
        void setEndFrame(int frameIndex);
        void setEndTime(float endTime);
        float getEndTime();

        void setLimits(float startTime, float endTime);

        bool isOwnedActions() const;
        void setOwnedActions(bool ownedActions);

        const std::string &getName() const;
        void setName(const std::string &name);

        void addActionFrame(float startTime, float endTime, Action* action, Object* object);
        void addActionFrame(float startTime, TimeAction* action, Object* object);
        ActionFrame getActionFrame(unsigned int index);
        void clearActionFrames();

        virtual bool run();
        //TODO: pause
        virtual bool stop();
*/
    };
}

#endif //ANIMATION_H