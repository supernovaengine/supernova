//
// (c) 2021 Eduardo Doria.
//

#include "TimedAction.h"
#include "util/Function.h"

using namespace Supernova;

TimedAction::TimedAction(Scene* scene): Action(scene){
    addComponent<TimedActionComponent>({});
}

void TimedAction::setAction(float duration, bool loop){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.duration = duration;
    timedAction.loop = loop;
    timedAction.timecount = 0;
    timedAction.time = 0;
    timedAction.value = 0;
}

void TimedAction::setFunction(std::function<float(float)> function){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.function.remove();
    timedAction.function = function;
}

void TimedAction::setFunction(sol::function function){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.function.remove();
    timedAction.function = function;
}

void TimedAction::setFunctionType(int functionType){
    TimedActionComponent& timedAction = getComponent<TimedActionComponent>();

    timedAction.function.remove();
    timedAction.function = Ease::getFunction(functionType);
}