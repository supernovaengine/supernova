//
// (c) 2024 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"

#include "util/Angle.h"
#include "util/Base64.h"
#include "util/Color.h"
#include "util/ResourceProgress.h"

using namespace Supernova;

void LuaBinding::registerUtilClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("ResourceType")
        .addVariable("Shader", ResourceType::Shader)
        .addVariable("Texture", ResourceType::Texture)
        .addVariable("Model", ResourceType::Model)
        .addVariable("Audio", ResourceType::Audio)
        .addVariable("Material", ResourceType::Material)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<Angle>("Angle")
        .addStaticFunction("radToDefault", &Angle::radToDefault)
        .addStaticFunction("degToDefault", &Angle::degToDefault)
        .addStaticFunction("defaultToRad", &Angle::defaultToRad)
        .addStaticFunction("defaultToDeg", &Angle::defaultToDeg)
        .addStaticFunction("radToDeg", &Angle::radToDeg)
        .addStaticFunction("degToRad", &Angle::degToRad)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Base64>("Base64")
        //.addStaticFunction("encode", &Base64::encode)  //TODO: read and write data in Lua
        //.addStaticFunction("decode", &Base64::decode)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Color>("Color")
        .addStaticFunction("linearTosRGB", 
            luabridge::overload<Vector3>(&Color::linearTosRGB),
            luabridge::overload<Vector4>(&Color::linearTosRGB),
            luabridge::overload<const float, const float, const float>(&Color::linearTosRGB),
            luabridge::overload<const float, const float, const float, const float>(&Color::linearTosRGB))
        .addStaticFunction("sRGBToLinear", 
            luabridge::overload<Vector3>(&Color::sRGBToLinear),
            luabridge::overload<Vector4>(&Color::sRGBToLinear),
            luabridge::overload<const float, const float, const float>(&Color::sRGBToLinear),
            luabridge::overload<const float, const float, const float, const float>(&Color::sRGBToLinear))
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ResourceBuildInfo>("ResourceBuildInfo")
        .addProperty("type", &ResourceBuildInfo::type)
        .addProperty("name", &ResourceBuildInfo::name)
        .addProperty("progress", &ResourceBuildInfo::progress)
        .addProperty("isActive", &ResourceBuildInfo::isActive)
        .addProperty("startTime", &ResourceBuildInfo::startTime)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<OverallBuildProgress>("OverallBuildProgress")
        .addProperty("totalProgress", &OverallBuildProgress::totalProgress)
        .addProperty("totalBuilds", &OverallBuildProgress::totalBuilds)
        .addProperty("completedBuilds", &OverallBuildProgress::completedBuilds)
        .addProperty("currentBuildName", &OverallBuildProgress::currentBuildName)
        .addProperty("currentBuildType", &OverallBuildProgress::currentBuildType)
        .addProperty("hasActiveBuilds", &OverallBuildProgress::hasActiveBuilds)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ResourceProgress>("ResourceProgress")
        .addStaticFunction("startBuild", &ResourceProgress::startBuild)
        .addStaticFunction("updateProgress", &ResourceProgress::updateProgress)
        .addStaticFunction("completeBuild", &ResourceProgress::completeBuild)
        .addStaticFunction("failBuild", &ResourceProgress::failBuild)
        .addStaticFunction("hasActiveBuilds", &ResourceProgress::hasActiveBuilds)
        .addStaticFunction("getOverallProgress", &ResourceProgress::getOverallProgress)
        .addStaticFunction("getActiveBuildCount", &ResourceProgress::getActiveBuildCount)
        .addStaticFunction("getAllActiveBuilds", &ResourceProgress::getAllActiveBuilds)
        .addStaticFunction("getCurrentBuild", &ResourceProgress::getCurrentBuild)
        .addStaticFunction("getResourceTypeName", &ResourceProgress::getResourceTypeName)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}