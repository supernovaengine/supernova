//
// (c) 2019 Eduardo Doria.
//

#ifndef MORPHACTION_H
#define MORPHACTION_H

#include "TimeAction.h"

namespace Supernova {

    class MorphAction: public TimeAction {

    protected:
        float endWeight;
        float startWeight;

        int morphIndex;

        bool objectStartWeight;

    public:
        MorphAction(int morphIndex, float duration, bool loop=false);
        MorphAction(int morphIndex, float endWeight, float duration, bool loop=false);
        MorphAction(int morphIndex, float startWeight, float endWeight, float duration, bool loop=false);
        virtual ~MorphAction();

        void setMorphIndex(int morphIndex);
        void setEndWeight(float endWeight);
        void setStartWeight(float startWeight);

        virtual bool run();

        virtual bool update(float interval);
    };

}


#endif //MORPHACTION_H
