//
// (c) 2023 Eduardo Doria.
//

#ifndef ACTION_H
#define ACTION_H

#include "object/EntityHandle.h"

namespace Supernova{
    class Action: public EntityHandle{
    public:
        Action(Scene* scene);
        Action(Scene* scene, Entity entity);
        virtual ~Action();

        void start();
        void pause();
        void stop();

        void setTarget(Entity target);
        Entity getTarget() const;

        void setSpeed(float speed);
        float getSpeed() const;

        bool isRunning() const;
        bool isStopped() const;
        bool isPaused() const;
    };
}

#endif //ACTION_H