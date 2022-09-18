//
// (c) 2021 Eduardo Doria.
//

#include "TimedAction.h"

using namespace Supernova;

TimedAction::TimedAction(Scene* scene): Action(scene){
    addComponent<TimedActionComponent>({});
}

void TimedAction::setAction(float duration, bool loop){
    ActionComponent& action = getComponent<ActionComponent>();
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    action.timecount = 0;
    timedAction.duration = duration;
    timedAction.loop = loop;
    timedAction.time = 0;
    timedAction.value = 0;
}

void TimedAction::setFunction(std::function<float(float)> function){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.function = function;
}

void TimedAction::setFunctionType(EaseType functionType){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.function = Ease::getFunction(functionType);
}