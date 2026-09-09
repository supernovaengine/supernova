// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "PositionAction.h"

using namespace doriax;

PositionAction::PositionAction(Scene* scene): TimedAction(scene){
    addComponent<PositionActionComponent>();
}

PositionAction::PositionAction(Scene* scene, Entity entity): TimedAction(scene, entity){

}

void PositionAction::setAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    PositionActionComponent& posAction = getComponent<PositionActionComponent>();

    posAction.startPosition = startPosition;
    posAction.endPosition = endPosition;
}