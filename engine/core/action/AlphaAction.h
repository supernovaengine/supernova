#ifndef ALPHAACTION_H
#define ALPHAACTION_H

#include "TimeAction.h"

namespace Supernova{

    class AlphaAction: public TimeAction{

    protected:
        float endAlpha;
        float startAlpha;

        bool objectStartAlpha;

    public:
        AlphaAction(float endAlpha, float duration, bool loop=false);
        AlphaAction(float startAlpha, float endAlpha, float duration, bool loop=false);
        virtual ~AlphaAction();

        virtual bool run();

        virtual bool step();
    };
}

#endif //ALPHAACTION_H
