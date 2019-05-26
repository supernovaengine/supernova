//
// (c) 2019 Eduardo Doria.
//

#ifndef MORPHTRACKS_H
#define MORPHTRACKS_H

#include "KeyframeTrack.h"

namespace Supernova {
    class MorphTracks: public KeyframeTrack {

    protected:
        std::vector<float> values;
        int morphIndex;

    public:
        MorphTracks();
        MorphTracks(int morphIndex, std::vector<float> times, std::vector<float> values);

        void setMorphIndex(int morphIndex);
        void setValues(std::vector<float> values);

        void addKeyframe(float time, float value);

        virtual bool update(float interval);
    };
}


#endif //MORPHTRACKS_H
