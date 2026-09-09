// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TRANSLATETRACKS_H
#define TRANSLATETRACKS_H

#include "action/Action.h"
#include "action/Ease.h"

namespace doriax{
    class DORIAX_API TranslateTracks: public Action{

    public:
        TranslateTracks(Scene* scene);
        TranslateTracks(Scene* scene, Entity entity);
        TranslateTracks(Scene* scene, std::vector<float> times, std::vector<Vector3> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<Vector3> values);
        void setEasings(std::vector<EaseType> easings);
        void setEasing(unsigned int segment, EaseType ease);
    };
}

#endif //TRANSLATETRACKS_H