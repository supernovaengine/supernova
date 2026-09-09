// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ROTATETRACKS_H
#define ROTATETRACKS_H

#include "action/Action.h"
#include "action/Ease.h"

namespace doriax{
    class DORIAX_API RotateTracks: public Action{

    public:
        RotateTracks(Scene* scene);
        RotateTracks(Scene* scene, Entity entity);
        RotateTracks(Scene* scene, std::vector<float> times, std::vector<Quaternion> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<Quaternion> values);
        void setEasings(std::vector<EaseType> easings);
        void setEasing(unsigned int segment, EaseType ease);
    };
}

#endif //ROTATETRACKS_H