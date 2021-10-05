//
// (c) 2021 Eduardo Doria.
//

#include "TimedAction.h"
#include "util/Function.h"

using namespace Supernova;

TimedAction::TimedAction(Scene* scene): Action(scene){
    addComponent<TimedActionComponent>({});
    addComponent<EaseComponent>({});

    EaseComponent& ease = getComponent<EaseComponent>();
    ease.function = Ease::linear;
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
    EaseComponent& ease = getComponent<EaseComponent>();

    ease.function.remove();
    ease.function = function;
}

int TimedAction::setFunction(lua_State* L){
    EaseComponent& ease = getComponent<EaseComponent>();

    ease.function.remove();
    return ease.function.set(L);
}

void TimedAction::setFunctionType(int functionType){
    EaseComponent& ease = getComponent<EaseComponent>();

    ease.function.remove();

    if (functionType == S_LINEAR){
        ease.function = Ease::linear;
    }else if(functionType == S_EASE_QUAD_IN){
        ease.function = Ease::easeInQuad;
    }else if(functionType == S_EASE_QUAD_OUT){
        ease.function = Ease::easeOutQuad;
    }else if(functionType == S_EASE_QUAD_IN_OUT){
        ease.function = Ease::easeInOutQuad;
    }else if(functionType == S_EASE_CUBIC_IN){
        ease.function = Ease::easeInCubic;
    }else if(functionType == S_EASE_CUBIC_OUT){
        ease.function = Ease::easeOutCubic;
    }else if(functionType == S_EASE_CUBIC_IN_OUT){
        ease.function = Ease::easeInOutCubic;
    }else if(functionType == S_EASE_QUART_IN){
        ease.function = Ease::easeInQuart;
    }else if(functionType == S_EASE_QUART_OUT){
        ease.function = Ease::easeOutQuart;
    }else if(functionType == S_EASE_QUART_IN_OUT){
        ease.function = Ease::easeInOutQuart;
    }else if(functionType == S_EASE_QUINT_IN){
        ease.function = Ease::easeInQuint;
    }else if(functionType == S_EASE_QUINT_OUT){
        ease.function = Ease::easeOutQuint;
    }else if(functionType == S_EASE_QUINT_IN_OUT){
        ease.function = Ease::easeInOutQuint;
    }else if(functionType == S_EASE_SINE_IN){
        ease.function = Ease::easeInSine;
    }else if(functionType == S_EASE_SINE_OUT){
        ease.function = Ease::easeOutSine;
    }else if(functionType == S_EASE_SINE_IN_OUT){
        ease.function = Ease::easeInOutSine;
    }else if(functionType == S_EASE_EXPO_IN){
        ease.function = Ease::easeInExpo;
    }else if(functionType == S_EASE_EXPO_OUT){
        ease.function = Ease::easeOutExpo;
    }else if(functionType == S_EASE_EXPO_IN_OUT){
        ease.function = Ease::easeInOutExpo;
    }else if(functionType == S_EASE_CIRC_IN){
        ease.function = Ease::easeInCirc;
    }else if(functionType == S_EASE_CIRC_OUT){
        ease.function = Ease::easeOutCirc;
    }else if(functionType == S_EASE_CIRC_IN_OUT){
        ease.function = Ease::easeInOutCirc;
    }else if(functionType == S_EASE_ELASTIC_IN){
        ease.function = Ease::easeInElastic;
    }else if(functionType == S_EASE_ELASTIC_OUT){
        ease.function = Ease::easeOutElastic;
    }else if(functionType == S_EASE_ELASTIC_IN_OUT){
        ease.function = Ease::easeInOutElastic;
    }else if(functionType == S_EASE_BACK_IN){
        ease.function = Ease::easeInBack;
    }else if(functionType == S_EASE_BACK_OUT){
        ease.function = Ease::easeOutBack;
    }else if(functionType == S_EASE_BACK_IN_OUT){
        ease.function = Ease::easeInOutBack;
    }else if(functionType == S_EASE_BOUNCE_IN){
        ease.function = Ease::easeInBounce;
    }else if(functionType == S_EASE_BOUNCE_OUT){
        ease.function = Ease::easeOutBounce;
    }else if(functionType == S_EASE_BOUNCE_IN_OUT){
        ease.function = Ease::easeInOutBounce;
    }

}