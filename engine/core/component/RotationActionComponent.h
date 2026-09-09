// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ROTATIONACTION_COMPONENT_H
#define ROTATIONACTION_COMPONENT_H

#include "math/Quaternion.h"

namespace doriax{

    struct DORIAX_API RotationActionComponent{
        Quaternion endRotation;
        Quaternion startRotation;

        bool shortestPath = false;
    };

}

#endif //ROTATIONACTION_COMPONENT_H