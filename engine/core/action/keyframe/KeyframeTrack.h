//
// (c) 2019 Eduardo Doria.
//

#ifndef KEYFRAMETRACK_H
#define KEYFRAMETRACK_H

#include "action/TimeAction.h"

namespace Supernova {

    class KeyframeTrack: public TimeAction {

    protected:
        std::vector<float> times;
        int index;
        float progress;

    public:
        KeyframeTrack();
        KeyframeTrack(std::vector<float> times);

        void setTimes(std::vector<float> times);

        virtual bool stop();

        virtual bool update(float interval);

    };

}


#endif //KEYFRAMETRACK_H
