// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AlphaAction.h"

using namespace doriax;

AlphaAction::AlphaAction(Scene* scene): TimedAction(scene){
    addComponent<AlphaActionComponent>();
}

AlphaAction::AlphaAction(Scene* scene, Entity entity): TimedAction(scene, entity){

}

void AlphaAction::setAction(float startAlpha, float endAlpha, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    AlphaActionComponent& alphaAction = getComponent<AlphaActionComponent>();

    alphaAction.startAlpha = startAlpha;
    alphaAction.endAlpha = endAlpha;
}
