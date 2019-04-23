//
// (c) 2019 Eduardo Doria.
//

#ifndef SCALETRACKS_H
#define SCALETRACKS_H

#include "KeyframeTrack.h"
#include "math/Vector3.h"

namespace Supernova {

    class ScaleTracks: public KeyframeTrack   {

    protected:
        std::vector<Vector3> values;

    public:
        ScaleTracks();
        ScaleTracks(std::vector<float> times, std::vector<Vector3> values);

        void setValues(std::vector<Vector3> values);

        void addKeyframe(float time, Vector3 value);

        virtual bool update(float interval);
    };

}


#endif //SCALETRACKS_H
