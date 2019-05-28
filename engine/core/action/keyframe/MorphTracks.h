//
// (c) 2019 Eduardo Doria.
//

#ifndef MORPHTRACKS_H
#define MORPHTRACKS_H

#include "KeyframeTrack.h"

namespace Supernova {
    class MorphTracks: public KeyframeTrack {

    protected:
        std::vector<std::vector<float>> values;

    public:
        MorphTracks();
        MorphTracks(std::vector<float> times, std::vector<std::vector<float>> values);

        void setValues(std::vector<std::vector<float>> values);

        void addKeyframe(float time, std::vector<float>);

        virtual bool update(float interval);
    };
}


#endif //MORPHTRACKS_H
