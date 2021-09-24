//
// (c) 2021 Eduardo Doria.
//

#include "PositionAction.h"

using namespace Supernova;

PositionAction::PositionAction(Scene* scene): TimedAction(scene){
    addComponent<PositionActionComponent>({});
}

void PositionAction::setAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.duration = duration;
    timedAction.loop = loop;
    timedAction.time = 0;
    timedAction.value = 0;

    PositionActionComponent& posAction = getComponent<PositionActionComponent>();

    posAction.startPosition = startPosition;
    posAction.endPosition = endPosition;
}