
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
        RotateAction(Quaternion endRotation, float duration, bool loop=false);
        RotateAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop=false);
        RotateAction(float endAngle2D, float duration, bool loop=false);
        RotateAction(float startAngle2D, float endAngle2D, float duration, bool loop=false);
        virtual ~RotateAction();

        virtual bool run();

        virtual bool update(float interval);
    };

}


#endif
