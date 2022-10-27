//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "EnumWrapper.h"

#include "ecs/Entity.h"
#include "ecs/Signature.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentArray.h"

//TODO: Add all components and properties
#include "component/ActionComponent.h"
#include "component/TimedActionComponent.h"
#include "component/UIComponent.h"
#include "component/ButtonComponent.h"
#include "component/ParticlesAnimationComponent.h"
#include "component/AudioComponent.h"

using namespace Supernova;

namespace luabridge
{
    // Entity type is not necessary here because it already exists Stack<unsigned int>

    template<> struct Stack<AudioState> : EnumWrapper<AudioState>{};
    template<> struct Stack<Audio3DAttenuation> : EnumWrapper<Audio3DAttenuation>{};

    template <>
    struct Stack <Signature>
    {
        static Result push(lua_State* L, Signature sig)
        {
            lua_pushstring(L, sig.to_string().c_str());
            return {};
        }

        static TypeResult<Signature> get(lua_State* L, int index)
        {
            if (lua_type(L, index) != LUA_TSTRING)
                return makeErrorCode(ErrorCode::InvalidTypeCast);
            return Signature(lua_tostring(L, index));
        }

        static bool isInstance (lua_State* L, int index)
        {
            return lua_type(L, index) == LUA_TSTRING;
        }
    };
}

void LuaBinding::registerECSClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("AudioState")
        .addProperty("Playing", AudioState::Playing)
        .addProperty("Paused", AudioState::Paused)
        .addProperty("Stopped", AudioState::Stopped)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Audio3DAttenuation")
        .addProperty("NO_ATTENUATION", Audio3DAttenuation::NO_ATTENUATION)
        .addProperty("INVERSE_DISTANCE", Audio3DAttenuation::INVERSE_DISTANCE)
        .addProperty("LINEAR_DISTANCE", Audio3DAttenuation::LINEAR_DISTANCE)
        .addProperty("EXPONENTIAL_DISTANCE", Audio3DAttenuation::EXPONENTIAL_DISTANCE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<EntityManager>("EntityManager")
        .addConstructor <void (*) (void)> ()
        .addFunction("createEntity", &EntityManager::createEntity)
        .addFunction("destroy", &EntityManager::destroy)
        .addFunction("setSignature", &EntityManager::setSignature)
        .addFunction("getSignature", &EntityManager::getSignature)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ActionComponent>("ActionComponent")
        .addProperty("target", &ActionComponent::target)
        .addProperty("state", &ActionComponent::state)
        .addProperty("timecount", &ActionComponent::timecount)
        .addProperty("speed", &ActionComponent::speed)
        .addProperty("startTrigger", &ActionComponent::startTrigger)
        .addProperty("stopTrigger", &ActionComponent::stopTrigger)
        .addProperty("pauseTrigger", &ActionComponent::pauseTrigger)
        .addProperty("onStart", [] (ActionComponent* self, lua_State* L) { return &self->onStart; }, [] (ActionComponent* self, lua_State* L) { self->onStart = L; })
        .addProperty("onPause", [] (ActionComponent* self, lua_State* L) { return &self->onPause; }, [] (ActionComponent* self, lua_State* L) { self->onPause = L; })
        .addProperty("onStop", [] (ActionComponent* self, lua_State* L) { return &self->onStop; }, [] (ActionComponent* self, lua_State* L) { self->onStop = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<TimedActionComponent>("TimedActionComponent")
        .addProperty("time", &TimedActionComponent::time)
        .addProperty("value", &TimedActionComponent::value)
        .addProperty("duration", &TimedActionComponent::duration)
        .addProperty("loop", &TimedActionComponent::loop)
        .addProperty("function", [] (TimedActionComponent* self, lua_State* L) { return &self->function; }, [] (TimedActionComponent* self, lua_State* L) { self->function = L; })
        .endClass();
    
    luabridge::getGlobalNamespace(L)
        .beginClass<UIComponent>("UIComponent")
        .addProperty("width", &UIComponent::width)
        .addProperty("height", &UIComponent::height)
        .addProperty("focused", &UIComponent::focused)
        .addProperty("loaded", &UIComponent::loaded)
        //.addProperty("buffer", &UIComponent::buffer)
        //.addProperty("indices", &UIComponent::indices)
        .addProperty("minBufferCount", &UIComponent::minBufferCount)
        .addProperty("minIndicesCount", &UIComponent::minIndicesCount)
        //.addProperty("render", &UIComponent::render)
        //.addProperty("shader", &UIComponent::shader)
        //.addProperty("shaderProperties", &UIComponent::shaderProperties)
        //.addProperty("slotVSParams", &UIComponent::slotVSParams)
        //.addProperty("slotFSParams", &UIComponent::slotFSParams)
        .addProperty("primitiveType", &UIComponent::primitiveType)
        .addProperty("vertexCount", &UIComponent::vertexCount)
        .addProperty("texture", &UIComponent::texture)
        .addProperty("color", &UIComponent::color)
        .addProperty("onMouseMove", [] (UIComponent* self, lua_State* L) { return &self->onMouseMove; }, [] (UIComponent* self, lua_State* L) { self->onMouseMove = L; })
        .addProperty("mouseMoved", &UIComponent::mouseMoved)
        .addProperty("needUpdateBuffer", &UIComponent::needUpdateBuffer)
        .addProperty("needUpdateTexture", &UIComponent::needUpdateTexture)
        .addProperty("needReload", &UIComponent::needReload)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ButtonComponent>("ButtonComponent")
        .addProperty("label", &ButtonComponent::label)
        .addProperty("textureNormal", &ButtonComponent::textureNormal)
        .addProperty("texturePressed", &ButtonComponent::texturePressed)
        .addProperty("textureDisabled", &ButtonComponent::textureDisabled)
        .addProperty("onPress", [] (ButtonComponent* self, lua_State* L) { return &self->onPress; }, [] (ButtonComponent* self, lua_State* L) { self->onPress = L; })
        .addProperty("onRelease", [] (ButtonComponent* self, lua_State* L) { return &self->onRelease; }, [] (ButtonComponent* self, lua_State* L) { self->onRelease = L; })
        .addProperty("pressed", &ButtonComponent::pressed)
        .addProperty("disabled", &ButtonComponent::disabled)
        .addProperty("needUpdateButton", &ButtonComponent::needUpdateButton)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleLifeInitializer>("ParticleLifeInitializer")
        .addProperty("minLife", &ParticleLifeInitializer::minLife)
        .addProperty("maxLife", &ParticleLifeInitializer::maxLife)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticlePositionInitializer>("ParticlePositionInitializer")
        .addProperty("minPosition", &ParticlePositionInitializer::minPosition)
        .addProperty("maxPosition", &ParticlePositionInitializer::maxPosition)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticlePositionModifier>("ParticlePositionModifier")
        .addProperty("fromTime", &ParticlePositionModifier::fromTime)
        .addProperty("toTime", &ParticlePositionModifier::toTime)
        .addProperty("fromPosition", &ParticlePositionModifier::fromPosition)
        .addProperty("toPosition", &ParticlePositionModifier::toPosition)
        .addProperty("function", [] (ParticlePositionModifier* self, lua_State* L) { return &self->function; }, [] (ParticlePositionModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleVelocityInitializer>("ParticleVelocityInitializer")
        .addProperty("minVelocity", &ParticleVelocityInitializer::minVelocity)
        .addProperty("maxVelocity", &ParticleVelocityInitializer::maxVelocity)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleVelocityModifier>("ParticleVelocityModifier")
        .addProperty("fromTime", &ParticleVelocityModifier::fromTime)
        .addProperty("toTime", &ParticleVelocityModifier::toTime)
        .addProperty("fromVelocity", &ParticleVelocityModifier::fromVelocity)
        .addProperty("toVelocity", &ParticleVelocityModifier::toVelocity)
        .addProperty("function", [] (ParticleVelocityModifier* self, lua_State* L) { return &self->function; }, [] (ParticleVelocityModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleAccelerationInitializer>("ParticleAccelerationInitializer")
        .addProperty("minAcceleration", &ParticleAccelerationInitializer::minAcceleration)
        .addProperty("maxAcceleration", &ParticleAccelerationInitializer::maxAcceleration)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleAccelerationModifier>("ParticleAccelerationModifier")
        .addProperty("fromTime", &ParticleAccelerationModifier::fromTime)
        .addProperty("toTime", &ParticleAccelerationModifier::toTime)
        .addProperty("fromAcceleration", &ParticleAccelerationModifier::fromAcceleration)
        .addProperty("toAcceleration", &ParticleAccelerationModifier::toAcceleration)
        .addProperty("function", [] (ParticleAccelerationModifier* self, lua_State* L) { return &self->function; }, [] (ParticleAccelerationModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleColorInitializer>("ParticleColorInitializer")
        .addProperty("minColor", &ParticleColorInitializer::minColor)
        .addProperty("maxColor", &ParticleColorInitializer::maxColor)
        .addProperty("useSRGB", &ParticleColorInitializer::useSRGB)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleColorModifier>("ParticleColorModifier")
        .addProperty("fromTime", &ParticleColorModifier::fromTime)
        .addProperty("toTime", &ParticleColorModifier::toTime)
        .addProperty("fromColor", &ParticleColorModifier::fromColor)
        .addProperty("toColor", &ParticleColorModifier::toColor)
        .addProperty("function", [] (ParticleColorModifier* self, lua_State* L) { return &self->function; }, [] (ParticleColorModifier* self, lua_State* L) { self->function = L; })
        .addProperty("useSRGB", &ParticleColorModifier::useSRGB)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleAlphaInitializer>("ParticleAlphaInitializer")
        .addProperty("minAlpha", &ParticleAlphaInitializer::minAlpha)
        .addProperty("maxAlpha", &ParticleAlphaInitializer::maxAlpha)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleAlphaModifier>("ParticleAlphaModifier")
        .addProperty("fromTime", &ParticleAlphaModifier::fromTime)
        .addProperty("toTime", &ParticleAlphaModifier::toTime)
        .addProperty("fromAlpha", &ParticleAlphaModifier::fromAlpha)
        .addProperty("toAlpha", &ParticleAlphaModifier::toAlpha)
        .addProperty("function", [] (ParticleAlphaModifier* self, lua_State* L) { return &self->function; }, [] (ParticleAlphaModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSizeInitializer>("ParticleSizeInitializer")
        .addProperty("minSize", &ParticleSizeInitializer::minSize)
        .addProperty("maxSize", &ParticleSizeInitializer::maxSize)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSizeModifier>("ParticleSizeModifier")
        .addProperty("fromTime", &ParticleSizeModifier::fromTime)
        .addProperty("toTime", &ParticleSizeModifier::toTime)
        .addProperty("fromSize", &ParticleSizeModifier::fromSize)
        .addProperty("toSize", &ParticleSizeModifier::toSize)
        .addProperty("function", [] (ParticleSizeModifier* self, lua_State* L) { return &self->function; }, [] (ParticleSizeModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSpriteInitializer>("ParticleSpriteInitializer")
        .addProperty("frames", &ParticleSpriteInitializer::frames)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSpriteModifier>("ParticleSpriteModifier")
        .addProperty("fromTime", &ParticleSpriteModifier::fromTime)
        .addProperty("toTime", &ParticleSpriteModifier::toTime)
        .addProperty("frames", &ParticleSpriteModifier::frames)
        .addProperty("function", [] (ParticleSpriteModifier* self, lua_State* L) { return &self->function; }, [] (ParticleSpriteModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleRotationInitializer>("ParticleRotationInitializer")
        .addProperty("minRotation", &ParticleRotationInitializer::minRotation)
        .addProperty("maxRotation", &ParticleRotationInitializer::maxRotation)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleRotationModifier>("ParticleRotationModifier")
        .addProperty("fromTime", &ParticleRotationModifier::fromTime)
        .addProperty("toTime", &ParticleRotationModifier::toTime)
        .addProperty("fromRotation", &ParticleRotationModifier::fromRotation)
        .addProperty("toRotation", &ParticleRotationModifier::toRotation)
        .addProperty("function", [] (ParticleRotationModifier* self, lua_State* L) { return &self->function; }, [] (ParticleRotationModifier* self, lua_State* L) { self->function = L; })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticlesAnimationComponent>("ParticlesAnimationComponent")
        .addProperty("newParticlesCount", &ParticlesAnimationComponent::newParticlesCount)
        .addProperty("emitter", &ParticlesAnimationComponent::emitter)
        .addProperty("rate", &ParticlesAnimationComponent::rate)
        .addProperty("lastUsedParticle", &ParticlesAnimationComponent::lastUsedParticle)
        .addProperty("lifeInitializer", &ParticlesAnimationComponent::lifeInitializer)
        .addProperty("lifeInitializer", &ParticlesAnimationComponent::lifeInitializer)
        .addProperty("positionInitializer", &ParticlesAnimationComponent::positionInitializer)
        .addProperty("positionModifier", &ParticlesAnimationComponent::positionModifier)
        .addProperty("velocityInitializer", &ParticlesAnimationComponent::velocityInitializer)
        .addProperty("velocityModifier", &ParticlesAnimationComponent::velocityModifier)
        .addProperty("accelerationInitializer", &ParticlesAnimationComponent::accelerationInitializer)
        .addProperty("accelerationModifier", &ParticlesAnimationComponent::accelerationModifier)
        .addProperty("colorInitializer", &ParticlesAnimationComponent::colorInitializer)
        .addProperty("colorModifier", &ParticlesAnimationComponent::colorModifier)
        .addProperty("alphaInitializer", &ParticlesAnimationComponent::alphaInitializer)
        .addProperty("alphaModifier", &ParticlesAnimationComponent::alphaModifier)
        .addProperty("sizeInitializer", &ParticlesAnimationComponent::sizeInitializer)
        .addProperty("sizeModifier", &ParticlesAnimationComponent::sizeModifier)
        .addProperty("spriteInitializer", &ParticlesAnimationComponent::spriteInitializer)
        .addProperty("spriteModifier", &ParticlesAnimationComponent::spriteModifier)
        .addProperty("rotationInitializer", &ParticlesAnimationComponent::rotationInitializer)
        .addProperty("rotationModifier", &ParticlesAnimationComponent::rotationModifier)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<AudioComponent>("AudioComponent")
        .addProperty("handle", &AudioComponent::handle)
        .addProperty("state", &AudioComponent::state)
        .addProperty("filename", &AudioComponent::filename)
        .addProperty("loaded", &AudioComponent::loaded)
        .addProperty("enableClocked", &AudioComponent::enableClocked)
        .addProperty("enable3D", &AudioComponent::enable3D)
        .addProperty("lastPosition", &AudioComponent::lastPosition)
        .addProperty("startTrigger", &AudioComponent::startTrigger)
        .addProperty("stopTrigger", &AudioComponent::stopTrigger)
        .addProperty("pauseTrigger", &AudioComponent::pauseTrigger)
        .addProperty("onStart", [] (AudioComponent* self, lua_State* L) { return &self->onStart; }, [] (AudioComponent* self, lua_State* L) { self->onStart = L; })
        .addProperty("onPause", [] (AudioComponent* self, lua_State* L) { return &self->onPause; }, [] (AudioComponent* self, lua_State* L) { self->onPause = L; })
        .addProperty("onStop", [] (AudioComponent* self, lua_State* L) { return &self->onStop; }, [] (AudioComponent* self, lua_State* L) { self->onStop = L; })
        .addProperty("volume", &AudioComponent::volume)
        .addProperty("pan", &AudioComponent::pan)
        .addProperty("looping", &AudioComponent::looping)
        .addProperty("loopingPoint", &AudioComponent::loopingPoint)
        .addProperty("protectVoice", &AudioComponent::protectVoice)
        .addProperty("inaudibleBehaviorMustTick", &AudioComponent::inaudibleBehaviorMustTick)
        .addProperty("inaudibleBehaviorKill", &AudioComponent::inaudibleBehaviorKill)
        .addProperty("minDistance", &AudioComponent::minDistance)
        .addProperty("maxDistance", &AudioComponent::maxDistance)
        .addProperty("attenuationModel", &AudioComponent::attenuationModel)
        .addProperty("attenuationRolloffFactor", &AudioComponent::attenuationRolloffFactor)
        .addProperty("dopplerFactor", &AudioComponent::dopplerFactor)
        .addProperty("length", &AudioComponent::length)
        .addProperty("playingTime", &AudioComponent::playingTime)
        .addProperty("needUpdate", &AudioComponent::needUpdate)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}