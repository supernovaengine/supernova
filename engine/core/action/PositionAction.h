// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef POSITIONACTION_H
#define POSITIONACTION_H

#include "TimedAction.h"

namespace doriax{
    class DORIAX_API PositionAction: public TimedAction{

    public:
        PositionAction(Scene* scene);
        PositionAction(Scene* scene, Entity entity);

        void setAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop=false);
    };
}

#endif //POSITIONACTION_H