//
// (c) 2024 Eduardo Doria.
//

#ifndef ROTATETRACKS_COMPONENT_H
#define ROTATETRACKS_COMPONENT_H

#include "Engine.h"

namespace Supernova{

    struct RotateTracksComponent{
        std::vector<Quaternion> values;
    };

}

#endif //ROTATETRACKS_COMPONENT_H