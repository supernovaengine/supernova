// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "RotationAction.h"

using namespace doriax;

RotationAction::RotationAction(Scene* scene): TimedAction(scene){
    addComponent<RotationActionComponent>();
}

RotationAction::RotationAction(Scene* scene, Entity entity): TimedAction(scene, entity){

}

void RotationAction::setAction(Quaternion startRotation, Quaternion endRotation, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    RotationActionComponent& rotAction = getComponent<RotationActionComponent>();

    rotAction.startRotation = startRotation;
    rotAction.endRotation = endRotation;
}