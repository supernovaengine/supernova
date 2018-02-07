#ifndef SCALEACTION_H
#define SCALEACTION_H

#include "TimeAction.h"
#include "math/Vector3.h"

namespace Supernova{

    class ScaleAction: public TimeAction {

    protected:
        Vector3 endScale;
        Vector3 startScale;

        bool objectStartScale;

    public:
        ScaleAction(Vector3 endScale, float duration, bool loop=false);
        ScaleAction(Vector3 startScale, Vector3 endScalel, float duration, bool loop=false);
        virtual ~ScaleAction();

        virtual bool run();

        virtual bool step();

    };

}


#endif //SCALEACTION_H
