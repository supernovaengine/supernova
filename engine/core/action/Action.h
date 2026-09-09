// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ACTION_H
#define ACTION_H

#include "object/EntityHandle.h"
#include "object/Object.h"

namespace doriax{
    class DORIAX_API Action: public EntityHandle{
    public:
        Action(Scene* scene);
        Action(Scene* scene, Entity entity);
        virtual ~Action();

        void start();
        void pause();
        void stop();

        void setOwnedTarget(bool ownedTarget);
        bool getOwnedTarget() const;

        void setTarget(Object* target);
        void setTarget(Entity target);
        Entity getTarget() const;

        void setSpeed(float speed);
        float getSpeed() const;

        void setWeight(float weight);
        float getWeight() const;

        bool isRunning() const;
        bool isStopped() const;
        bool isPaused() const;

        float getTimeCount() const;
    };
}

#endif //ACTION_H