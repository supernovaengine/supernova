// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SCALETRACKS_H
#define SCALETRACKS_H

#include "action/Action.h"
#include "action/Ease.h"

namespace doriax{
    class DORIAX_API ScaleTracks: public Action{

    public:
        ScaleTracks(Scene* scene);
        ScaleTracks(Scene* scene, Entity entity);
        ScaleTracks(Scene* scene, std::vector<float> times, std::vector<Vector3> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<Vector3> values);
        void setEasings(std::vector<EaseType> easings);
        void setEasing(unsigned int segment, EaseType ease);
    };
}

#endif //SCALETRACKS_H