
#ifndef ROTATEACTION_H
#define ROTATEACTION_H

#include "TimeAction.h"
#include "math/Quaternion.h"

namespace Supernova {

    class RotateAction : public TimeAction {

    protected:
        Quaternion endRotation;
        Quaternion startRotation;

        bool objectStartRotation;

    public:
        RotateAction(Quaternion endRotation, float duration, bool loop);
        RotateAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop);
        virtual ~RotateAction();

        virtual bool run();

        virtual bool step();
    };

}


#endif
