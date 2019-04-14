//
// (c) 2019 Eduardo Doria.
//

#ifndef ANIMATION_H
#define ANIMATION_H

#include "TimeAction.h"
#include <map>

namespace Supernova {

    class Animation: public Action {

        struct ActionFrame{
            float startTime;
            TimeAction* action;
        };

    private:

        std::vector<ActionFrame> actions;
        bool ownedActions;
        bool loop;

        float startTime;
        float endTime;

    public:

        Animation();
        Animation(bool loop);
        virtual ~Animation();

        bool isLoop();
        void setLoop(bool loop);

        void setStartTime(float startTime);
        float getStartTime();

        void setEndTime(float endTime);
        float getEndTime();

        void setLimits(float startTime, float endTime);

        bool isOwnedActions() const;
        void setOwnedActions(bool ownedActions);

        void addActionFrame(float startTime, TimeAction* action, Object* object);
        void clearActionFrames();

        virtual bool run();
        virtual bool stop();
        virtual bool step();

    };

}


#endif //ANIMATION_H
