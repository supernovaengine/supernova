//
// (c) 2019 Eduardo Doria.
//

#ifndef ROTATETRACKS_H
#define ROTATETRACKS_H

#include "KeyframeTrack.h"
#include "math/Quaternion.h"

namespace Supernova {

    class RotateTracks: public KeyframeTrack  {

    protected:
        std::vector<Quaternion> values;

    public:
        RotateTracks();
        RotateTracks(std::vector<float> times, std::vector<Quaternion> values);

        void setValues(std::vector<Quaternion> values);

        void addKeyframe(float time, Quaternion value);

        virtual bool update(float interval);
    };

}

#endif //ROTATETRACKS_H
