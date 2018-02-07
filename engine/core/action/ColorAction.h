#ifndef COLORACTION_H
#define COLORACTION_H

#include "TimeAction.h"
#include "math/Vector4.h"

namespace Supernova{

    class ColorAction: public TimeAction{

    protected:
        Vector4 endColor;
        Vector4 startColor;

        bool objectStartColor;

    public:
        ColorAction(Vector4 endColor, float duration, bool loop=false);
        ColorAction(Vector4 startColor, Vector4 endColor, float duration, bool loop=false);
        ColorAction(float endRed, float endGreen, float endBlue, float duration, bool loop=false);
        ColorAction(float startRed, float startGreen, float startBlue, float endRed, float endGreen, float endBlue, float duration, bool loop=false);
        virtual ~ColorAction();

        virtual bool run();

        virtual bool step();
    };
}


#endif //COLORACTION_H
