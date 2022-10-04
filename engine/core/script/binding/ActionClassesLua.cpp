//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "EnumWrapper.h"

#include "action/Action.h"
#include "action/TimedAction.h"
#include "action/AlphaAction.h"
#include "action/ColorAction.h"
#include "action/PositionAction.h"
#include "action/ScaleAction.h"
#include "action/Animation.h"
#include "action/ParticlesAnimation.h"
#include "action/SpriteAnimation.h"
#include "action/Ease.h"
#include "action/keyframe/MorphTracks.h"
#include "action/keyframe/RotateTracks.h"
#include "action/keyframe/ScaleTracks.h"
#include "action/keyframe/TranslateTracks.h"

using namespace Supernova;

namespace luabridge
{
    template<> struct Stack<EaseType> : EnumWrapper<EaseType>{};
}

void LuaBinding::registerActionClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("EaseType")
        .addProperty("LINEAR", EaseType::LINEAR)
        .addProperty("QUAD_IN", EaseType::QUAD_IN)
        .addProperty("QUAD_OUT", EaseType::QUAD_OUT)
        .addProperty("QUAD_IN_OUT", EaseType::QUAD_IN_OUT)
        .addProperty("CUBIC_IN", EaseType::CUBIC_IN)
        .addProperty("CUBIC_OUT", EaseType::CUBIC_OUT)
        .addProperty("CUBIC_IN_OUT", EaseType::CUBIC_IN_OUT)
        .addProperty("QUART_IN", EaseType::QUART_IN)
        .addProperty("QUART_OUT", EaseType::QUART_OUT)
        .addProperty("QUART_IN_OUT", EaseType::QUART_IN_OUT)
        .addProperty("QUINT_IN", EaseType::QUINT_IN)
        .addProperty("QUINT_OUT", EaseType::QUINT_OUT)
        .addProperty("QUINT_IN_OUT", EaseType::QUINT_IN_OUT)
        .addProperty("SINE_IN", EaseType::SINE_IN)
        .addProperty("SINE_OUT", EaseType::SINE_OUT)
        .addProperty("SINE_IN_OUT", EaseType::SINE_IN_OUT)
        .addProperty("EXPO_IN", EaseType::EXPO_IN)
        .addProperty("EXPO_OUT", EaseType::EXPO_OUT)
        .addProperty("EXPO_IN_OUT", EaseType::EXPO_IN_OUT)
        .addProperty("CIRC_IN", EaseType::CIRC_IN)
        .addProperty("CIRC_OUT", EaseType::CIRC_OUT)
        .addProperty("CIRC_IN_OUT", EaseType::CIRC_IN_OUT)
        .addProperty("ELASTIC_IN", EaseType::ELASTIC_IN)
        .addProperty("ELASTIC_OUT", EaseType::ELASTIC_OUT)
        .addProperty("ELASTIC_IN_OUT", EaseType::ELASTIC_IN_OUT)
        .addProperty("BACK_IN", EaseType::BACK_IN)
        .addProperty("BACK_OUT", EaseType::BACK_OUT)
        .addProperty("BACK_IN_OUT", EaseType::BACK_IN_OUT)
        .addProperty("BOUNCE_IN", EaseType::BOUNCE_IN)
        .addProperty("BOUNCE_OUT", EaseType::BOUNCE_OUT)
        .addProperty("BOUNCE_IN_OUT", EaseType::BOUNCE_IN_OUT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<Action>("Action")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("start", &Action::start)
        .addFunction("pause", &Action::pause)
        .addProperty("target", &Action::getTarget, &Action::setTarget)
        .addProperty("speed", &Action::getSpeed, &Action::setSpeed)
        .addProperty("entity", &Action::getEntity)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<TimedAction, Action>("TimedAction")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setFunction", [] (TimedAction* self, lua_State* L) { self->setFunction(L); })
        .addFunction("setFunctionType", &TimedAction::setFunctionType)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<AlphaAction, TimedAction>("AlphaAction")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setAction", &AlphaAction::setAction)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<ColorAction, TimedAction>("ColorAction")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setAction", (void(ColorAction::*)(Vector3, Vector3, float, bool))&ColorAction::setAction)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<PositionAction, TimedAction>("PositionAction")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setAction", &PositionAction::setAction)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<ScaleAction, TimedAction>("ScaleAction")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setAction", &ScaleAction::setAction)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<ParticlesAnimation, Action>("ParticlesAnimation")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setLifeInitializer", (void(ParticlesAnimation::*)(float, float))&ParticlesAnimation::setLifeInitializer)
        .addFunction("setPositionInitializer", (void(ParticlesAnimation::*)(Vector3, Vector3))&ParticlesAnimation::setPositionInitializer)
        .addFunction("setPositionModifier", (void(ParticlesAnimation::*)(float, float, Vector3, Vector3, EaseType))&ParticlesAnimation::setPositionModifier)
        .addFunction("setVelocityInitializer", (void(ParticlesAnimation::*)(Vector3, Vector3))&ParticlesAnimation::setVelocityInitializer)
        .addFunction("setVelocityModifier", (void(ParticlesAnimation::*)(float, float, Vector3, Vector3, EaseType))&ParticlesAnimation::setVelocityModifier)
        .addFunction("setAccelerationInitializer", (void(ParticlesAnimation::*)(Vector3, Vector3))&ParticlesAnimation::setAccelerationInitializer)
        .addFunction("setAccelerationModifier", (void(ParticlesAnimation::*)(float, float, Vector3, Vector3, EaseType))&ParticlesAnimation::setAccelerationModifier)
        .addFunction("setColorInitializer", (void(ParticlesAnimation::*)(Vector3, Vector3))&ParticlesAnimation::setColorInitializer)
        .addFunction("setColorModifier", (void(ParticlesAnimation::*)(float, float, Vector3, Vector3, EaseType))&ParticlesAnimation::setColorModifier)
        .addFunction("setAlphaInitializer", (void(ParticlesAnimation::*)(float, float))&ParticlesAnimation::setAlphaInitializer)
        .addFunction("setAlphaModifier", (void(ParticlesAnimation::*)(float, float, float, float, EaseType))&ParticlesAnimation::setAlphaModifier)
        .addFunction("setSizeInitializer", (void(ParticlesAnimation::*)(float, float))&ParticlesAnimation::setSizeInitializer)
        .addFunction("setSizeModifier", (void(ParticlesAnimation::*)(float, float, float, float, EaseType))&ParticlesAnimation::setSizeModifier)
        .addFunction("setSpriteIntializer", (void(ParticlesAnimation::*)(std::vector<int>))&ParticlesAnimation::setSpriteIntializer)
        .addFunction("setSpriteModifier", (void(ParticlesAnimation::*)(float, float, std::vector<int>, EaseType))&ParticlesAnimation::setSpriteModifier)
        .addFunction("setRotationInitializer", (void(ParticlesAnimation::*)(float, float))&ParticlesAnimation::setRotationInitializer)
        .addFunction("setRotationModifier", (void(ParticlesAnimation::*)(float, float, float, float, EaseType))&ParticlesAnimation::setRotationModifier)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<SpriteAnimation, Action>("SpriteAnimation")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setAnimation", (void(SpriteAnimation::*)(std::vector<int>, std::vector<int>, bool))&SpriteAnimation::setAnimation)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Animation, Action>("Animation")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("loop", &Animation::isLoop, &Animation::setLoop)
        .addProperty("endTime", &Animation::getEndTime, &Animation::setEndTime)
        .addProperty("ownedActions", &Animation::isOwnedActions, &Animation::setOwnedActions)
        .addProperty("name", &Animation::getName, &Animation::setName)
        .addFunction("addActionFrame", (void(Animation::*)(float, float, Entity, Entity))&Animation::addActionFrame)
        .addFunction("getActionFrame", &Animation::getActionFrame)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Ease>("Ease")
        .addStaticFunction("linear", &Ease::linear)
        .addStaticFunction("easeInQuad", &Ease::easeInQuad)
        .addStaticFunction("easeOutQuad", &Ease::easeOutQuad)
        .addStaticFunction("easeInOutQuad", &Ease::easeInOutQuad)
        .addStaticFunction("easeInCubic", &Ease::easeInCubic)
        .addStaticFunction("easeOutCubic", &Ease::easeOutCubic)
        .addStaticFunction("easeInOutCubic", &Ease::easeInOutCubic)
        .addStaticFunction("easeInQuart", &Ease::easeInQuart)
        .addStaticFunction("easeOutQuart", &Ease::easeOutQuart)
        .addStaticFunction("easeInQuint", &Ease::easeOutQuint)
        .addStaticFunction("easeOutQuint", &Ease::easeOutQuint)
        .addStaticFunction("easeInOutQuint", &Ease::easeInOutQuint)
        .addStaticFunction("easeInSine", &Ease::easeInSine)
        .addStaticFunction("easeOutSine", &Ease::easeOutSine)
        .addStaticFunction("easeInOutSine", &Ease::easeInOutSine)
        .addStaticFunction("easeInExpo", &Ease::easeInExpo)
        .addStaticFunction("easeOutExpo", &Ease::easeOutExpo)
        .addStaticFunction("easeInOutExpo", &Ease::easeInOutExpo)
        .addStaticFunction("easeInCirc", &Ease::easeInCirc)
        .addStaticFunction("easeOutCirc", &Ease::easeOutCirc)
        .addStaticFunction("easeInOutCirc", &Ease::easeInOutCirc)
        .addStaticFunction("easeInElastic", &Ease::easeInElastic)
        .addStaticFunction("easeOutElastic", &Ease::easeOutElastic)
        .addStaticFunction("easeInOutElastic", &Ease::easeInOutElastic)
        .addStaticFunction("easeInBack", &Ease::easeInBack)
        .addStaticFunction("easeOutBack", &Ease::easeOutBack)
        .addStaticFunction("easeInOutBack", &Ease::easeInOutBack)
        .addStaticFunction("easeOutBounce", &Ease::easeOutBounce)
        .addStaticFunction("easeInBounce", &Ease::easeInBounce)
        .addStaticFunction("easeInOutBounce", &Ease::easeInOutBounce)

        .addStaticFunction("getFunction", &Ease::getFunction)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<MorphTracks, Action>("MorphTracks")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setTimes", &MorphTracks::setTimes)
        .addFunction("setValues", &MorphTracks::setValues)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<RotateTracks, Action>("RotateTracks")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setTimes", &RotateTracks::setTimes)
        .addFunction("setValues", &RotateTracks::setValues)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<ScaleTracks, Action>("ScaleTracks")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setTimes", &ScaleTracks::setTimes)
        .addFunction("setValues", &ScaleTracks::setValues)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<TranslateTracks, Action>("TranslateTracks")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setTimes", &TranslateTracks::setTimes)
        .addFunction("setValues", &TranslateTracks::setValues)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}