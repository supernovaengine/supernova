//
// (c) 2021 Eduardo Doria.
//

#ifndef ALPHAACTION_H
#define ALPHAACTION_H

#include "TimedAction.h"

namespace Supernova{
    class AlphaAction: public TimedAction{

    public:
        AlphaAction(Scene* scene);

        void setAction(float startAlpha, float endAlpha, float duration, bool loop=false);
    };
}

#endif //ALPHAACTION_H