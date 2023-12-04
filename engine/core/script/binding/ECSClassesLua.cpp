//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"
#include "LuaBridgeAddon.h"

#include "ecs/Entity.h"
#include "ecs/Signature.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentArray.h"

//TODO: Add all systems
#include "subsystem/AudioSystem.h"
#include "subsystem/PhysicsSystem.h"

//TODO: Add all components and properties
#include "component/ActionComponent.h"
#include "component/TimedActionComponent.h"
#include "component/UIComponent.h"
#include "component/UILayoutComponent.h"
#include "component/UIContainerComponent.h"
#include "component/ButtonComponent.h"
#include "component/ParticlesAnimationComponent.h"
#include "component/AudioComponent.h"
#include "component/SpriteComponent.h"
#include "component/FogComponent.h"
#include "component/TilemapComponent.h"

using namespace Supernova;

void LuaBinding::registerECSClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("AudioState")
        .addProperty("Playing", AudioState::Playing)
        .addProperty("Paused", AudioState::Paused)
        .addProperty("Stopped", AudioState::Stopped)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("AudioAttenuation")
        .addProperty("NO_ATTENUATION", AudioAttenuation::NO_ATTENUATION)
        .addProperty("INVERSE_DISTANCE", AudioAttenuation::INVERSE_DISTANCE)
        .addProperty("LINEAR_DISTANCE", AudioAttenuation::LINEAR_DISTANCE)
        .addProperty("EXPONENTIAL_DISTANCE", AudioAttenuation::EXPONENTIAL_DISTANCE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("AnchorPreset")
        .addProperty("NONE", AnchorPreset::NONE)
        .addProperty("TOP_LEFT", AnchorPreset::TOP_LEFT)
        .addProperty("TOP_RIGHT", AnchorPreset::TOP_RIGHT)
        .addProperty("BOTTOM_LEFT", AnchorPreset::BOTTOM_LEFT)
        .addProperty("BOTTOM_RIGHT", AnchorPreset::BOTTOM_RIGHT)
        .addProperty("CENTER_LEFT", AnchorPreset::CENTER_LEFT)
        .addProperty("CENTER_TOP", AnchorPreset::CENTER_TOP)
        .addProperty("CENTER_RIGHT", AnchorPreset::CENTER_RIGHT)
        .addProperty("CENTER_BOTTOM", AnchorPreset::CENTER_BOTTOM)
        .addProperty("CENTER", AnchorPreset::CENTER)
        .addProperty("LEFT_WIDE", AnchorPreset::LEFT_WIDE)
        .addProperty("TOP_WIDE", AnchorPreset::TOP_WIDE)
        .addProperty("RIGHT_WIDE", AnchorPreset::RIGHT_WIDE)
        .addProperty("BOTTOM_WIDE", AnchorPreset::BOTTOM_WIDE)
        .addProperty("VERTICAL_CENTER_WIDE", AnchorPreset::VERTICAL_CENTER_WIDE)
        .addProperty("HORIZONTAL_CENTER_WIDE", AnchorPreset::HORIZONTAL_CENTER_WIDE)
        .addProperty("FULL_LAYOUT", AnchorPreset::FULL_LAYOUT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("PivotPreset")
        .addProperty("CENTER", PivotPreset::CENTER)
        .addProperty("TOP_CENTER", PivotPreset::TOP_CENTER)
        .addProperty("BOTTOM_CENTER", PivotPreset::BOTTOM_CENTER)
        .addProperty("LEFT_CENTER", PivotPreset::LEFT_CENTER)
        .addProperty("RIGHT_CENTER", PivotPreset::RIGHT_CENTER)
        .addProperty("TOP_LEFT", PivotPreset::TOP_LEFT)
        .addProperty("BOTTOM_LEFT", PivotPreset::BOTTOM_LEFT)
        .addProperty("TOP_RIGHT", PivotPreset::TOP_RIGHT)
        .addProperty("BOTTOM_RIGHT", PivotPreset::BOTTOM_RIGHT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("ContainerType")
        .addProperty("VERTICAL", ContainerType::VERTICAL)
        .addProperty("HORIZONTAL", ContainerType::HORIZONTAL)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("FogType")
        .addProperty("LINEAR", FogType::LINEAR)
        .addProperty("EXPONENTIAL", FogType::EXPONENTIAL)
        .addProperty("EXPONENTIALSQUARED", FogType::EXPONENTIALSQUARED)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<AudioSystem>("AudioSystem")
        .addStaticFunction("stopAll", &AudioSystem::stopAll)
        .addStaticFunction("pauseAll", &AudioSystem::pauseAll)
        .addStaticFunction("resumeAll", &AudioSystem::resumeAll)
        .addStaticFunction("checkActive", &AudioSystem::checkActive)
        .addStaticProperty("globalVolume", &AudioSystem::getGlobalVolume,  &AudioSystem::setGlobalVolume)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<PhysicsSystem>("PhysicsSystem")
        .addProperty("pointsToMeterScale", &PhysicsSystem::getPointsToMeterScale,  &PhysicsSystem::setPointsToMeterScale)
        .addProperty("beginContact2D", [] (PhysicsSystem* self, lua_State* L) { return &self->beginContact2D; }, [] (PhysicsSystem* self, lua_State* L) { self->beginContact2D = L; })
        .addProperty("endContact2D", [] (PhysicsSystem* self, lua_State* L) { return &self->endContact2D; }, [] (PhysicsSystem* self, lua_State* L) { self->endContact2D = L; })
        .addProperty("preSolve2D", [] (PhysicsSystem* self, lua_State* L) { return &self->preSolve2D; }, [] (PhysicsSystem* self, lua_State* L) { self->preSolve2D = L; })
        .addProperty("postSolve2D", [] (PhysicsSystem* self, lua_State* L) { return &self->postSolve2D; }, [] (PhysicsSystem* self, lua_State* L) { self->postSolve2D = L; })
        .addProperty("shouldCollide2D", [] (PhysicsSystem* self, lua_State* L) { return &self->shouldCollide2D; }, [] (PhysicsSystem* self, lua_State* L) { self->shouldCollide2D = L; })

        .addProperty("onBodyActivated3D", [] (PhysicsSystem* self, lua_State* L) { return &self->onBodyActivated3D; }, [] (PhysicsSystem* self, lua_State* L) { self->onBodyActivated3D = L; })
        .addProperty("onBodyDeactivated3D", [] (PhysicsSystem* self, lua_State* L) { return &self->onBodyDeactivated3D; }, [] (PhysicsSystem* self, lua_State* L) { self->onBodyDeactivated3D = L; })
        .addProperty("onContactAdded3D", [] (PhysicsSystem* self, lua_State* L) { return &self->onContactAdded3D; }, [] (PhysicsSystem* self, lua_State* L) { self->onContactAdded3D = L; })
        .addProperty("onContactPersisted3D", [] (PhysicsSystem* self, lua_State* L) { return &self->onContactPersisted3D; }, [] (PhysicsSystem* self, lua_State* L) { self->onContactPersisted3D = L; })
        .addProperty("onContactRemoved3D", [] (PhysicsSystem* self, lua_State* L) { return &self->onContactRemoved3D; }, [] (PhysicsSystem* self, lua_State* L) { self->onContactRemoved3D = L; })
        .addProperty("shouldCollide3D", [] (PhysicsSystem* self, lua_State* L) { return &self->shouldCollide3D; }, [] (PhysicsSystem* self, lua_State* L) { self->shouldCollide3D = L; })

        .addFunction("createBody2D", &PhysicsSystem::createBody2D)
        .addFunction("removeBody2D", &PhysicsSystem::removeBody2D)

        .addFunction("createRectShape2D", &PhysicsSystem::createRectShape2D)
        .addFunction("createCenteredRectShape2D", &PhysicsSystem::createCenteredRectShape2D)
        .addFunction("createPolygonShape2D", &PhysicsSystem::createPolygonShape2D)
        .addFunction("createCircleShape2D", &PhysicsSystem::createCircleShape2D)
        .addFunction("createTwoSidedEdgeShape2D", &PhysicsSystem::createTwoSidedEdgeShape2D)
        .addFunction("createOneSidedEdgeShape2D", &PhysicsSystem::createOneSidedEdgeShape2D)
        .addFunction("createLoopChainShape2D", &PhysicsSystem::createLoopChainShape2D)
        .addFunction("createChainShape2D", &PhysicsSystem::createChainShape2D)
        .addFunction("removeAllShapes2D", &PhysicsSystem::removeAllShapes2D)

        .addFunction("createBody3D", &PhysicsSystem::createBody3D)
        .addFunction("removeBody3D", &PhysicsSystem::removeBody3D)
        .addFunction("createBoxShape3D", &PhysicsSystem::createBoxShape3D)
        .addFunction("createSphereShape3D", &PhysicsSystem::createSphereShape3D)
        .addFunction("createCapsuleShape3D", &PhysicsSystem::createCapsuleShape3D)
        .addFunction("createTaperedCapsuleShape3D", &PhysicsSystem::createTaperedCapsuleShape3D)
        .addFunction("createCylinderShape3D", &PhysicsSystem::createCylinderShape3D)
        .addFunction("createConvexHullShape3D", &PhysicsSystem::createConvexHullShape3D)
        .addFunction("createMeshShape3D", 
            luabridge::overload<Entity , std::vector<Vector3>, std::vector<uint16_t>>(&PhysicsSystem::createMeshShape3D),
            luabridge::overload<Entity, MeshComponent&>(&PhysicsSystem::createMeshShape3D))
        .addFunction("createHeightFieldShape3D", &PhysicsSystem::createHeightFieldShape3D)

        .addFunction("loadBody2D", &PhysicsSystem::loadBody2D)
        .addFunction("destroyBody2D", &PhysicsSystem::destroyBody2D)
        .addFunction("destroyBody3D", &PhysicsSystem::destroyBody3D)
        .addFunction("loadShape2D", &PhysicsSystem::loadShape2D)
        .addFunction("destroyShape2D", &PhysicsSystem::destroyShape2D)

        .addFunction("loadDistanceJoint2D", &PhysicsSystem::loadDistanceJoint2D)
        .addFunction("loadRevoluteJoint2D", &PhysicsSystem::loadRevoluteJoint2D)
        .addFunction("loadPrismaticJoint2D", &PhysicsSystem::loadPrismaticJoint2D)
        .addFunction("loadPulleyJoint2D", &PhysicsSystem::loadPulleyJoint2D)
        .addFunction("loadGearJoint2D", &PhysicsSystem::loadGearJoint2D)
        .addFunction("loadMouseJoint2D", &PhysicsSystem::loadMouseJoint2D)
        .addFunction("loadWheelJoint2D", &PhysicsSystem::loadWheelJoint2D)
        .addFunction("loadWeldJoint2D", &PhysicsSystem::loadWeldJoint2D)
        .addFunction("loadFrictionJoint2D", &PhysicsSystem::loadFrictionJoint2D)
        .addFunction("loadMotorJoint2D", &PhysicsSystem::loadMotorJoint2D)
        .addFunction("loadRopeJoint2D", &PhysicsSystem::loadRopeJoint2D)
        .addFunction("destroyJoint2D", &PhysicsSystem::destroyJoint2D)

        .addFunction("loadFixedJoint3D", &PhysicsSystem::loadFixedJoint3D)
        .addFunction("loadDistanceJoint3D", &PhysicsSystem::loadDistanceJoint3D)
        .addFunction("loadPointJoint3D", &PhysicsSystem::loadPointJoint3D)
        .addFunction("loadHingeJoint3D", &PhysicsSystem::loadHingeJoint3D)
        .addFunction("loadConeJoint3D", &PhysicsSystem::loadConeJoint3D)
        .addFunction("loadPrismaticJoint3D", &PhysicsSystem::loadPrismaticJoint3D)
        .addFunction("loadSwingTwistJoint3D", &PhysicsSystem::loadSwingTwistJoint3D)
        .addFunction("loadSixDOFJoint3D", &PhysicsSystem::loadSixDOFJoint3D)
        .addFunction("loadPathJoint3D", &PhysicsSystem::loadPathJoint3D)
        .addFunction("loadGearJoint3D", &PhysicsSystem::loadGearJoint3D)
        .addFunction("loadRackAndPinionJoint3D", &PhysicsSystem::loadRackAndPinionJoint3D)
        .addFunction("loadPulleyJoint3D", &PhysicsSystem::loadPulleyJoint3D)
        .addFunction("destroyJoint3D", &PhysicsSystem::destroyJoint3D)
        .endClass();

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
        .addProperty("onStep", [] (ActionComponent* self, lua_State* L) { return &self->onStep; }, [] (ActionComponent* self, lua_State* L) { self->onStep = L; })
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
        .beginClass<UILayoutComponent>("UILayoutComponent")
        .addProperty("width", &UILayoutComponent::width)
        .addProperty("height", &UILayoutComponent::height)
        .addProperty("anchorPointLeft", &UILayoutComponent::anchorPointLeft)
        .addProperty("anchorPointTop", &UILayoutComponent::anchorPointTop)
        .addProperty("anchorPointRight", &UILayoutComponent::anchorPointRight)
        .addProperty("anchorPointBottom", &UILayoutComponent::anchorPointBottom)
        .addProperty("anchorOffsetLeft", &UILayoutComponent::anchorOffsetLeft)
        .addProperty("anchorOffsetTop", &UILayoutComponent::anchorOffsetTop)
        .addProperty("anchorOffsetRight", &UILayoutComponent::anchorOffsetRight)
        .addProperty("anchorOffsetBottom", &UILayoutComponent::anchorOffsetBottom)
        .addProperty("anchorPreset", &UILayoutComponent::anchorPreset)
        .addProperty("usingAnchors", &UILayoutComponent::usingAnchors)
        .addProperty("containerBoxIndex", &UILayoutComponent::containerBoxIndex)
        .addProperty("scissor", &UILayoutComponent::scissor)
        .addProperty("ignoreScissor", &UILayoutComponent::ignoreScissor)
        .addProperty("needUpdateSizes", &UILayoutComponent::needUpdateSizes)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<UIComponent>("UIComponent")
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
        .addProperty("onGetFocus", [] (UIComponent* self, lua_State* L) { return &self->onGetFocus; }, [] (UIComponent* self, lua_State* L) { self->onGetFocus = L; })
        .addProperty("onLostFocus", [] (UIComponent* self, lua_State* L) { return &self->onLostFocus; }, [] (UIComponent* self, lua_State* L) { self->onLostFocus = L; })
        .addProperty("onPointerMove", [] (UIComponent* self, lua_State* L) { return &self->onPointerMove; }, [] (UIComponent* self, lua_State* L) { self->onPointerMove = L; })
        .addProperty("onPointerDown", [] (UIComponent* self, lua_State* L) { return &self->onPointerDown; }, [] (UIComponent* self, lua_State* L) { self->onPointerDown = L; })
        .addProperty("onPointerUp", [] (UIComponent* self, lua_State* L) { return &self->onPointerUp; }, [] (UIComponent* self, lua_State* L) { self->onPointerUp = L; })
        .addProperty("automaticFlipY", &UIComponent::automaticFlipY)
        .addProperty("flipY", &UIComponent::flipY)
        .addProperty("pointerMoved", &UIComponent::pointerMoved)
        .addProperty("focused", &UIComponent::focused)
        .addProperty("needReload", &UIComponent::needReload)
        .addProperty("needUpdateBuffer", &UIComponent::needUpdateBuffer)
        .addProperty("needUpdateTexture", &UIComponent::needUpdateTexture)
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
        .addProperty("lastUsedParticle", &ParticlesAnimationComponent::lastUsedParticle)
        .addProperty("emitter", &ParticlesAnimationComponent::emitter)
        .addProperty("loop", &ParticlesAnimationComponent::loop)
        .addProperty("rate", &ParticlesAnimationComponent::rate)
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

    luabridge::getGlobalNamespace(L)
        .beginClass<TileRectData>("TileRectData")
        .addProperty("name", &TileRectData::name)
        .addProperty("submeshId", &TileRectData::submeshId)
        .addProperty("rect", &TileRectData::rect)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<TileData>("TileData")
        .addProperty("name", &TileData::name)
        .addProperty("rectId", &TileData::rectId)
        .addProperty("position", &TileData::position)
        .addProperty("width", &TileData::width)
        .addProperty("height", &TileData::height)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<TilemapComponent>("TilemapComponent")
        .addProperty("width", &TilemapComponent::width)
        .addProperty("height", &TilemapComponent::height)
        .addProperty("automaticFlipY", &TilemapComponent::automaticFlipY)
        .addProperty("flipY", &TilemapComponent::flipY)
        .addProperty("reserveTiles", &TilemapComponent::reserveTiles)
        .addProperty("numTiles", &TilemapComponent::numTiles)
        //.addProperty("tilesRect", &TilemapComponent::tilesRect)
        //.addProperty("tiles", &TilemapComponent::tiles)
        .addProperty("needUpdateTilemap", &TilemapComponent::needUpdateTilemap)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}