//
// (c) 2024 Eduardo Doria.
//

#include "RotationAction.h"

using namespace Supernova;

RotationAction::RotationAction(Scene* scene): TimedAction(scene){
    addComponent<RotationActionComponent>({});
}

void RotationAction::setAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    RotationActionComponent& rotAction = getComponent<RotationActionComponent>();

    rotAction.startRotation = startRotation;
    rotAction.endRotation = endRotation;
}