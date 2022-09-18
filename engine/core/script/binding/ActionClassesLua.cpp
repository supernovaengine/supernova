//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

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

void LuaBinding::registerActionClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS
/*
    sol::state_view lua(L);

    auto action = lua.new_usertype<Action>("Action",
        sol::call_constructor, sol::constructors<Action(Scene*), Action(Scene*, Entity)>());

    action["start"] = &Action::start;
    action["pause"] = &Action::pause;
    action["target"] = sol::property(&Action::getTarget, &Action::setTarget);
    action["setTarget"] = &Action::setTarget;
    action["getTarget"] = &Action::getTarget;
    action["speed"] = sol::property(&Action::getSpeed, &Action::setSpeed);
    action["setSpeed"] = &Action::setSpeed;
    action["getSpeed"] = &Action::getSpeed;
    action["entity"] = sol::property(&Action::getEntity);
    action["getEntity"] = &Action::getEntity;


    auto timedaction = lua.new_usertype<TimedAction>("TimedAction",
        sol::call_constructor, sol::constructors<TimedAction(Scene*)>(),
        sol::base_classes, sol::bases<Action>());

    timedaction["setFunction"] = sol::resolve<void(sol::protected_function)>(&TimedAction::setFunction);
    timedaction["setFunctionType"] = &TimedAction::setFunctionType;


    auto alphaaction = lua.new_usertype<AlphaAction>("AlphaAction",
        sol::call_constructor, sol::constructors<AlphaAction(Scene*)>(),
        sol::base_classes, sol::bases<TimedAction>());

    alphaaction["setAction"] = &AlphaAction::setAction;


    auto coloraction = lua.new_usertype<ColorAction>("ColorAction",
        sol::call_constructor, sol::constructors<ColorAction(Scene*)>(),
        sol::base_classes, sol::bases<TimedAction>());

    coloraction["setAction"] = sol::overload(sol::resolve<void(Vector3, Vector3, float, bool)>(&ColorAction::setAction), sol::resolve<void(Vector4, Vector4, float, bool)>(&ColorAction::setAction));


    auto positionaction = lua.new_usertype<PositionAction>("PositionAction",
        sol::call_constructor, sol::constructors<PositionAction(Scene*)>(),
        sol::base_classes, sol::bases<TimedAction>());

    positionaction["setAction"] = &PositionAction::setAction;


    auto scaleaction = lua.new_usertype<ScaleAction>("ScaleAction",
        sol::call_constructor, sol::constructors<ScaleAction(Scene*)>(),
        sol::base_classes, sol::bases<TimedAction>());

    scaleaction["setAction"] = &ScaleAction::setAction;


    auto particlesanimation = lua.new_usertype<ParticlesAnimation>("ParticlesAnimation",
        sol::call_constructor, sol::constructors<ParticlesAnimation(Scene*)>(),
        sol::base_classes, sol::bases<Action>());

    particlesanimation["setLifeInitializer"] = sol::overload(sol::resolve<void(float)>(&ParticlesAnimation::setLifeInitializer), sol::resolve<void(float, float)>(&ParticlesAnimation::setLifeInitializer));
    particlesanimation["setPositionInitializer"] = sol::overload(sol::resolve<void(Vector3)>(&ParticlesAnimation::setPositionInitializer), sol::resolve<void(Vector3, Vector3)>(&ParticlesAnimation::setPositionInitializer));
    particlesanimation["setPositionModifier"] = sol::overload(sol::resolve<void(float, float, Vector3, Vector3)>(&ParticlesAnimation::setPositionModifier), sol::resolve<void(float, float, Vector3, Vector3, EaseType)>(&ParticlesAnimation::setPositionModifier));
    particlesanimation["setVelocityInitializer"] = sol::overload(sol::resolve<void(Vector3)>(&ParticlesAnimation::setVelocityInitializer), sol::resolve<void(Vector3, Vector3)>(&ParticlesAnimation::setVelocityInitializer));
    particlesanimation["setVelocityModifier"] = sol::overload(sol::resolve<void(float, float, Vector3, Vector3)>(&ParticlesAnimation::setVelocityModifier), sol::resolve<void(float, float, Vector3, Vector3, EaseType)>(&ParticlesAnimation::setVelocityModifier));
    particlesanimation["setAccelerationInitializer"] = sol::overload(sol::resolve<void(Vector3)>(&ParticlesAnimation::setAccelerationInitializer), sol::resolve<void(Vector3, Vector3)>(&ParticlesAnimation::setAccelerationInitializer));
    particlesanimation["setAccelerationModifier"] = sol::overload(sol::resolve<void(float, float, Vector3, Vector3)>(&ParticlesAnimation::setAccelerationModifier), sol::resolve<void(float, float, Vector3, Vector3, EaseType)>(&ParticlesAnimation::setAccelerationModifier));
    particlesanimation["setColorInitializer"] = sol::overload(sol::resolve<void(Vector3)>(&ParticlesAnimation::setColorInitializer), sol::resolve<void(Vector3, Vector3)>(&ParticlesAnimation::setColorInitializer));
    particlesanimation["setColorModifier"] = sol::overload(sol::resolve<void(float, float, Vector3, Vector3)>(&ParticlesAnimation::setColorModifier), sol::resolve<void(float, float, Vector3, Vector3, EaseType)>(&ParticlesAnimation::setColorModifier));
    particlesanimation["setAlphaInitializer"] = sol::overload(sol::resolve<void(float)>(&ParticlesAnimation::setAlphaInitializer), sol::resolve<void(float, float)>(&ParticlesAnimation::setAlphaInitializer));
    particlesanimation["setAlphaModifier"] = sol::overload(sol::resolve<void(float, float, float, float)>(&ParticlesAnimation::setAlphaModifier), sol::resolve<void(float, float, float, float, EaseType)>(&ParticlesAnimation::setAlphaModifier));
    particlesanimation["setSizeInitializer"] = sol::overload(sol::resolve<void(float)>(&ParticlesAnimation::setSizeInitializer), sol::resolve<void(float, float)>(&ParticlesAnimation::setSizeInitializer));
    particlesanimation["setSizeModifier"] = sol::overload(sol::resolve<void(float, float, float, float)>(&ParticlesAnimation::setSizeModifier), sol::resolve<void(float, float, float, float, EaseType)>(&ParticlesAnimation::setSizeModifier));
    particlesanimation["setSpriteIntializer"] = sol::overload(sol::resolve<void(std::vector<int>)>(&ParticlesAnimation::setSpriteIntializer), sol::resolve<void(int, int)>(&ParticlesAnimation::setSpriteIntializer));
    particlesanimation["setSpriteModifier"] = sol::overload(sol::resolve<void(float, float, std::vector<int>)>(&ParticlesAnimation::setSpriteModifier), sol::resolve<void(float, float, std::vector<int>, EaseType)>(&ParticlesAnimation::setSpriteModifier));
    particlesanimation["setRotationInitializer"] = sol::overload(sol::resolve<void(float)>(&ParticlesAnimation::setRotationInitializer), sol::resolve<void(float, float)>(&ParticlesAnimation::setRotationInitializer));
    particlesanimation["setRotationModifier"] = sol::overload(sol::resolve<void(float, float, float, float)>(&ParticlesAnimation::setRotationModifier), sol::resolve<void(float, float, float, float, EaseType)>(&ParticlesAnimation::setRotationModifier));


    auto spriteanimation = lua.new_usertype<SpriteAnimation>("SpriteAnimation",
        sol::call_constructor, sol::constructors<SpriteAnimation(Scene*)>(),
        sol::base_classes, sol::bases<Action>());

    spriteanimation["setAnimation"] = sol::overload(sol::resolve<void(std::vector<int>, std::vector<int>, bool)>(&SpriteAnimation::setAnimation), sol::resolve<void(int, int, int, bool)>(&SpriteAnimation::setAnimation));


    auto animation = lua.new_usertype<Animation>("Animation",
        sol::call_constructor, sol::constructors<Animation(Scene*), Animation(Scene*, Entity)>(),
        sol::base_classes, sol::bases<Action>());

    animation["loop"] = sol::property(&Animation::isLoop, &Animation::setLoop);
    animation["isLoop"] = &Animation::isLoop;
    animation["setLoop"] = &Animation::setLoop;
    animation["endTime"] = sol::property(&Animation::getEndTime, &Animation::setEndTime);
    animation["setEndTime"] = &Animation::setEndTime;
    animation["getEndTime"] = &Animation::getEndTime;
    animation["ownedActions"] = sol::property(&Animation::isOwnedActions, &Animation::setOwnedActions);
    animation["isOwnedActions"] = &Animation::isOwnedActions;
    animation["setOwnedActions"] = &Animation::setOwnedActions;
    animation["name"] = sol::property(&Animation::getName, &Animation::setName);
    animation["getName"] = &Animation::getName;
    animation["setName"] = &Animation::setName;
    animation["addActionFrame"] = sol::overload( sol::resolve<void(float, float, Entity, Entity)>(&Animation::addActionFrame), sol::resolve<void(float, Entity, Entity)>(&Animation::addActionFrame) );
    animation["getActionFrame"] = &Animation::getActionFrame;
 

    lua.new_enum("EaseType",
        "LINEAR", EaseType::LINEAR,
        "QUAD_IN", EaseType::QUAD_IN,
        "QUAD_OUT", EaseType::QUAD_OUT,
        "QUAD_IN_OUT", EaseType::QUAD_IN_OUT,
        "CUBIC_IN", EaseType::CUBIC_IN,
        "CUBIC_OUT", EaseType::CUBIC_OUT,
        "CUBIC_IN_OUT", EaseType::CUBIC_IN_OUT,
        "QUART_IN", EaseType::QUART_IN,
        "QUART_OUT", EaseType::QUART_OUT,
        "QUART_IN_OUT", EaseType::QUART_IN_OUT,
        "QUINT_IN", EaseType::QUINT_IN,
        "QUINT_OUT", EaseType::QUINT_OUT,
        "QUINT_IN_OUT", EaseType::QUINT_IN_OUT,
        "SINE_IN", EaseType::SINE_IN,
        "SINE_OUT", EaseType::SINE_OUT,
        "SINE_IN_OUT", EaseType::SINE_IN_OUT,
        "EXPO_IN", EaseType::EXPO_IN,
        "EXPO_OUT", EaseType::EXPO_OUT,
        "EXPO_IN_OUT", EaseType::EXPO_IN_OUT,
        "CIRC_IN", EaseType::CIRC_IN,
        "CIRC_OUT", EaseType::CIRC_OUT,
        "CIRC_IN_OUT", EaseType::CIRC_IN_OUT,
        "ELASTIC_IN", EaseType::ELASTIC_IN,
        "ELASTIC_OUT", EaseType::ELASTIC_OUT,
        "ELASTIC_IN_OUT", EaseType::ELASTIC_IN_OUT,
        "BACK_IN", EaseType::BACK_IN,
        "BACK_OUT", EaseType::BACK_OUT,
        "BACK_IN_OUT", EaseType::BACK_IN_OUT,
        "BOUNCE_IN", EaseType::BOUNCE_IN,
        "BOUNCE_OUT", EaseType::BOUNCE_OUT,
        "BOUNCE_IN_OUT", EaseType::BOUNCE_IN_OUT
        );

    auto ease = lua.new_usertype<Ease>("Ease",
        sol::no_constructor);

    ease["linear"] = Ease::linear;
    ease["easeInQuad"] = Ease::easeInQuad;
    ease["easeOutQuad"] = Ease::easeOutQuad;
    ease["easeInOutQuad"] = Ease::easeInOutQuad;
    ease["easeInCubic"] = Ease::easeInCubic;
    ease["easeOutCubic"] = Ease::easeOutCubic;
    ease["easeInOutCubic"] = Ease::easeInOutCubic;
    ease["easeInQuart"] = Ease::easeInQuart;
    ease["easeOutQuart"] = Ease::easeOutQuart;
    ease["easeInQuint"] = Ease::easeOutQuint;
    ease["easeOutQuint"] = Ease::easeOutQuint;
    ease["easeInOutQuint"] = Ease::easeInOutQuint;
    ease["easeInSine"] = Ease::easeInSine;
    ease["easeOutSine"] = Ease::easeOutSine;
    ease["easeInOutSine"] = Ease::easeInOutSine;
    ease["easeInExpo"] = Ease::easeInExpo;
    ease["easeOutExpo"] = Ease::easeOutExpo;
    ease["easeInOutExpo"] = Ease::easeInOutExpo;
    ease["easeInCirc"] = Ease::easeInCirc;
    ease["easeOutCirc"] = Ease::easeOutCirc;
    ease["easeInOutCirc"] = Ease::easeInOutCirc;
    ease["easeInElastic"] = Ease::easeInElastic;
    ease["easeOutElastic"] = Ease::easeOutElastic;
    ease["easeInOutElastic"] = Ease::easeInOutElastic;
    ease["easeInBack"] = Ease::easeInBack;
    ease["easeOutBack"] = Ease::easeOutBack;
    ease["easeInOutBack"] = Ease::easeInOutBack;
    ease["easeOutBounce"] = Ease::easeOutBounce;
    ease["easeInBounce"] = Ease::easeInBounce;
    ease["easeInOutBounce"] = Ease::easeInOutBounce;

    ease["getFunction"] = Ease::getFunction;


    auto morphtracks = lua.new_usertype<MorphTracks>("MorphTracks",
        sol::call_constructor, sol::constructors<MorphTracks(Scene*), MorphTracks(Scene*, std::vector<float>, std::vector<std::vector<float>>)>(),
        sol::base_classes, sol::bases<Action>());
    
    morphtracks["setTimes"] = &MorphTracks::setTimes;
    morphtracks["setValues"] = &MorphTracks::setValues;


    auto rotatetracks = lua.new_usertype<RotateTracks>("RotateTracks",
        sol::call_constructor, sol::constructors<RotateTracks(Scene*), RotateTracks(Scene*, std::vector<float>, std::vector<Quaternion>)>(),
        sol::base_classes, sol::bases<Action>());
    
    rotatetracks["setTimes"] = &RotateTracks::setTimes;
    rotatetracks["setValues"] = &RotateTracks::setValues;


    auto scaletracks = lua.new_usertype<ScaleTracks>("ScaleTracks",
        sol::call_constructor, sol::constructors<ScaleTracks(Scene*), ScaleTracks(Scene*, std::vector<float>, std::vector<Vector3>)>(),
        sol::base_classes, sol::bases<Action>());
    
    scaletracks["setTimes"] = &ScaleTracks::setTimes;
    scaletracks["setValues"] = &ScaleTracks::setValues;


    auto translatetracks = lua.new_usertype<TranslateTracks>("TranslateTracks",
        sol::call_constructor, sol::constructors<TranslateTracks(Scene*), TranslateTracks(Scene*, std::vector<float>, std::vector<Vector3>)>(),
        sol::base_classes, sol::bases<Action>());
    
    translatetracks["setTimes"] = &TranslateTracks::setTimes;
    translatetracks["setValues"] = &TranslateTracks::setValues;
*/
#endif //DISABLE_LUA_BINDINGS

}