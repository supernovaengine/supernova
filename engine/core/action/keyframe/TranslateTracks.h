//
// (c) 2019 Eduardo Doria.
//

#ifndef TRANSLATETRACKS_H
#define TRANSLATETRACKS_H

#include "KeyframeTrack.h"
#include "math/Vector3.h"

namespace Supernova {

    class TranslateTracks: public KeyframeTrack {

    protected:
        std::vector<Vector3> values;

    public:
        TranslateTracks();
        TranslateTracks(std::vector<float> times, std::vector<Vector3> values);

        void setValues(std::vector<Vector3> values);

        void addKeyframe(float time, Vector3 value);

        virtual bool update(float interval);
    };

}


#endif //TRANSLATETRACKS_H
