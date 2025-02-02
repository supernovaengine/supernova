//
// (c) 2024 Eduardo Doria.
//

#ifndef ROTATETRACKS_H
#define ROTATETRACKS_H

#include "action/Action.h"

namespace Supernova{
    class SUPERNOVA_API RotateTracks: public Action{

    public:
        RotateTracks(Scene* scene);
        RotateTracks(Scene* scene, std::vector<float> times, std::vector<Quaternion> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<Quaternion> values);
    };
}

#endif //ROTATETRACKS_H