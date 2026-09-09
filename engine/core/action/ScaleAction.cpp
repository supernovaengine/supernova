// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ScaleAction.h"

using namespace doriax;

ScaleAction::ScaleAction(Scene* scene): TimedAction(scene){
    addComponent<ScaleActionComponent>();
}

ScaleAction::ScaleAction(Scene* scene, Entity entity): TimedAction(scene, entity){

}

void ScaleAction::setAction(Vector3 startScale, Vector3 endScale, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    ScaleActionComponent& scaleAction = getComponent<ScaleActionComponent>();

    scaleAction.startScale = startScale;
    scaleAction.endScale = endScale;
}