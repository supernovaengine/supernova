// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MORPHTRACKS_H
#define MORPHTRACKS_H

#include "action/Action.h"
#include "action/Ease.h"

namespace doriax{
    class DORIAX_API MorphTracks: public Action{

    public:
        MorphTracks(Scene* scene);
        MorphTracks(Scene* scene, Entity entity);
        MorphTracks(Scene* scene, std::vector<float> times, std::vector<std::vector<float>> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<std::vector<float>> values);
        void setEasings(std::vector<EaseType> easings);
        void setEasing(unsigned int segment, EaseType ease);
    };
}

#endif //MORPHTRACKS_H