// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef ROTATIONACTION_H
#define ROTATIONACTION_H

#include "TimedAction.h"

namespace doriax{
    class DORIAX_API RotationAction: public TimedAction{

    public:
        RotationAction(Scene* scene);
        RotationAction(Scene* scene, Entity entity);

        void setAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop=false);
    };
}

#endif //ROTATIONACTION_H