//
// (c) 2021 Eduardo Doria.
//

#include "RotationAction.h"

using namespace Supernova;

RotationAction::RotationAction(Scene* scene): TimedAction(scene){
    addComponent<RotationActionComponent>({});
}

void RotationAction::setAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.duration = duration;
    timedAction.loop = loop;
    timedAction.time = 0;
    timedAction.value = 0;

    RotationActionComponent& rotAction = getComponent<RotationActionComponent>();

    rotAction.startRotation = startRotation;
    rotAction.endRotation = endRotation;
}