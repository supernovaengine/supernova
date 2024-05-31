//
// (c) 2024 Eduardo Doria.
//

#ifndef ROTATIONACTION_COMPONENT_H
#define ROTATIONACTION_COMPONENT_H

#include "math/Quaternion.h"

namespace Supernova{

    struct RotationActionComponent{
        Quaternion endRotation;
        Quaternion startRotation;
    };

}

#endif //ROTATIONACTION_COMPONENT_H