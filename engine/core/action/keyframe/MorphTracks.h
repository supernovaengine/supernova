//
// (c) 2024 Eduardo Doria.
//

#ifndef MORPHTRACKS_H
#define MORPHTRACKS_H

#include "action/Action.h"

namespace Supernova{
    class SUPERNOVA_API MorphTracks: public Action{

    public:
        MorphTracks(Scene* scene);
        MorphTracks(Scene* scene, std::vector<float> times, std::vector<std::vector<float>> values);

        void setTimes(std::vector<float> times);
        void setValues(std::vector<std::vector<float>> values);
    };
}

#endif //MORPHTRACKS_H