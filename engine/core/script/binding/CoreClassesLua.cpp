//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "EnumWrapper.h"

#include "Engine.h"
#include "Log.h"
#include "Scene.h"
#include "Log.h"
#include "Input.h"

namespace luabridge
{
    template<> struct Stack<Supernova::Scaling> : EnumWrapper<Supernova::Scaling>{};
    template<> struct Stack<Supernova::Platform> : EnumWrapper<Supernova::Platform>{};
    template<> struct Stack<Supernova::GraphicBackend> : EnumWrapper<Supernova::GraphicBackend>{};
    template<> struct Stack<Supernova::TextureStrategy> : EnumWrapper<Supernova::TextureStrategy>{};
    template<> struct Stack<Supernova::TextureType> : EnumWrapper<Supernova::TextureType>{};
    template<> struct Stack<Supernova::ColorFormat> : EnumWrapper<Supernova::ColorFormat>{};
    template<> struct Stack<Supernova::LightType> : EnumWrapper<Supernova::LightType>{};
}

using namespace Supernova;

void LuaBinding::registerCoreClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Scaling")
        .addProperty("FITWIDTH", Scaling::FITWIDTH)
        .addProperty("FITHEIGHT", Scaling::FITHEIGHT)
        .addProperty("LETTERBOX", Scaling::LETTERBOX)
        .addProperty("CROP", Scaling::CROP)
        .addProperty("STRETCH", Scaling::STRETCH)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Platform")
        .addProperty("MacOS", Platform::MacOS)
        .addProperty("iOS", Platform::iOS)
        .addProperty("Web", Platform::Web)
        .addProperty("Android", Platform::Android)
        .addProperty("Linux", Platform::Linux)
        .addProperty("Windows", Platform::Windows)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("GraphicBackend")
        .addProperty("GLCORE33", GraphicBackend::GLCORE33)
        .addProperty("GLES2", GraphicBackend::GLES2)
        .addProperty("GLES3", GraphicBackend::GLES3)
        .addProperty("D3D11", GraphicBackend::D3D11)
        .addProperty("METAL", GraphicBackend::METAL)
        .addProperty("WGPU", GraphicBackend::WGPU)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("TextureStrategy")
        .addProperty("FIT", TextureStrategy::FIT)
        .addProperty("RESAMPLE", TextureStrategy::RESAMPLE)
        .addProperty("NONE", TextureStrategy::NONE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("TextureType")
        .addProperty("TEXTURE_2D", TextureType::TEXTURE_2D)
        .addProperty("TEXTURE_3D", TextureType::TEXTURE_3D)
        .addProperty("TEXTURE_CUBE", TextureType::TEXTURE_CUBE)
        .addProperty("TEXTURE_ARRAY", TextureType::TEXTURE_ARRAY)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("ColorFormat")
        .addProperty("RED", ColorFormat::RED)
        .addProperty("RGBA", ColorFormat::RGBA)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("LightType")
        .addProperty("DIRECTIONAL", LightType::DIRECTIONAL)
        .addProperty("POINT", LightType::POINT)
        .addProperty("SPOT", LightType::SPOT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<Engine>("Engine")
        .addStaticProperty("scene", &Engine::getScene, &Engine::setScene)
        .addStaticFunction("setScene", &Engine::setScene)
        .addStaticFunction("getScene", &Engine::getScene)
        .addStaticFunction("addSceneLayer", &Engine::addSceneLayer)
        .addStaticProperty("canvasWidth", &Engine::getCanvasWidth)
        .addStaticProperty("canvasHeight", &Engine::getCanvasHeight)
        .addStaticFunction("setCanvasSize", &Engine::setCanvasSize)
        .addStaticProperty("preferedCanvasWidth", &Engine::getPreferedCanvasWidth)
        .addStaticProperty("preferedCanvasHeight", &Engine::getPreferedCanvasHeight)
        .addStaticFunction("calculateCanvas", &Engine::calculateCanvas)
        .addStaticProperty("viewRect", &Engine::getViewRect)
        .addStaticProperty("scalingMode", &Engine::getScalingMode, &Engine::setScalingMode)
        .addStaticProperty("textureStrategy", &Engine::getTextureStrategy, &Engine::setTextureStrategy)
        .addStaticProperty("callMouseInTouchEvent", &Engine::isCallMouseInTouchEvent, &Engine::setCallMouseInTouchEvent)
        .addStaticProperty("callTouchInMouseEvent", &Engine::isCallTouchInMouseEvent, &Engine::setCallTouchInMouseEvent)
        .addStaticFunction("setCallTouchInMouseEvent", &Engine::setCallTouchInMouseEvent)
        .addStaticProperty("useDegrees", &Engine::isUseDegrees, &Engine::setUseDegrees)
        .addStaticProperty("defaultNearestScaleTexture", &Engine::isDefaultNearestScaleTexture, &Engine::setDefaultNearestScaleTexture)
        .addStaticProperty("defaultResampleToPOTTexture", &Engine::isDefaultResampleToPOTTexture, &Engine::setDefaultResampleToPOTTexture)
        .addStaticProperty("automaticTransparency", &Engine::isAutomaticTransparency, &Engine::setAutomaticTransparency)
        .addStaticProperty("automaticFlipY", &Engine::isAutomaticFlipY, &Engine::setAutomaticFlipY)
        .addStaticProperty("allowEventsOutCanvas", &Engine::isAllowEventsOutCanvas, &Engine::setAllowEventsOutCanvas)
        .addStaticProperty("fixedTimeSceneUpdate", &Engine::isFixedTimeSceneUpdate, &Engine::setFixedTimeSceneUpdate)
        .addStaticProperty("updateTime", &Engine::getUpdateTime, &Engine::setUpdateTime)
        .addStaticFunction("setUpdateTimeMS", &Engine::setUpdateTimeMS)
        .addStaticProperty("sceneUpdateTime", &Engine::getSceneUpdateTime)
        .addStaticProperty("platform", &Engine::getPlatform)
        .addStaticProperty("graphicBackend", &Engine::getGraphicBackend)
        .addStaticProperty("openGL", &Engine::isOpenGL)
        .addStaticProperty("framerate", &Engine::getFramerate)
        .addStaticProperty("deltatime", &Engine::getDeltatime)


        .addStaticProperty("onViewLoaded", [] () { return &Engine::onViewLoaded; }, [] (lua_State* L) { Engine::onViewLoaded = L; })
        .addStaticProperty("onCanvasChanged", [] () { return &Engine::onViewChanged; }, [] (lua_State* L) { Engine::onViewChanged = L; })
        .addStaticProperty("onDraw", [] () { return &Engine::onDraw; }, [] (lua_State* L) { Engine::onDraw = L; })
        .addStaticProperty("onUpdate", [] () { return &Engine::onUpdate; }, [] (lua_State* L) { Engine::onUpdate = L; })
        //.addStaticProperty("onShutdown", [] () { return &Engine::onShutdown; }, [] (lua_State* L) { Engine::onShutdown = L; })
        //.addStaticProperty("onTouchStart", [] () { return &Engine::onTouchStart; }, [] (lua_State* L) { Engine::onTouchStart = L; })

        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void()>>("FunctionSubscribe_V")
        .addFunction("__call", &FunctionSubscribe<void()>::call)
        .addFunction("call", &FunctionSubscribe<void()>::call)
        .addFunction("add", (bool (FunctionSubscribe<void()>::*)(const std::string&, lua_State*))&FunctionSubscribe<void()>::add)
        .endClass();
/*
    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void(int,float,float)>>("FunctionSubscribe_V_IFF")
        .addFunction("__call", &FunctionSubscribe<void(int,float,float)>::call)
        .addFunction("call", &FunctionSubscribe<void(int,float,float)>::call)
        .addFunction("add", (bool (FunctionSubscribe<void(int,float,float)>::*)(const std::string&, lua_State*))&FunctionSubscribe<void(int,float,float)>::add)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void(int,float,float,int)>>("FunctionSubscribe_V_IFFI")
        .addFunction("__call", &FunctionSubscribe<void(int,float,float,int)>::call)
        .addFunction("call", &FunctionSubscribe<void(int,float,float,int)>::call)
        .addFunction("add", (bool (FunctionSubscribe<void(int,float,float,int)>::*)(const std::string&, lua_State*))&FunctionSubscribe<void(int,float,float,int)>::add)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void(int,bool,int)>>("FunctionSubscribe_V_IBI")
        .addFunction("__call", &FunctionSubscribe<void(int,bool,int)>::call)
        .addFunction("call", &FunctionSubscribe<void(int,bool,int)>::call)
        .addFunction("add", (bool (FunctionSubscribe<void(int,bool,int)>::*)(const std::string&, lua_State*))&FunctionSubscribe<void(int,bool,int)>::add)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void(wchar_t)>>("FunctionSubscribe_V_W")
        .addFunction("__call", &FunctionSubscribe<void(wchar_t)>::call)
        .addFunction("call", &FunctionSubscribe<void(wchar_t)>::call)
        .addFunction("add", (bool (FunctionSubscribe<void(wchar_t)>::*)(const std::string&, lua_State*))&FunctionSubscribe<void(wchar_t)>::add)
        .endClass();
*/
/*
    sol::state_view lua(L);


    auto engine = lua.new_usertype<Engine>("Engine",
            sol::no_constructor);

    //engine["scene"] = sol::property(&Engine::getScene, &Engine::setScene);
    engine["setScene"] = &Engine::setScene;
    engine["getScene"] = &Engine::getScene;
    engine["addSceneLayer"] = &Engine::addSceneLayer;
    engine["canvasWidth"] = sol::property(&Engine::getCanvasWidth);
    engine["canvasHeight"] = sol::property(&Engine::getCanvasHeight);
    engine["setCanvasSize"] = &Engine::setCanvasSize;
    engine["preferedCanvasWidth"] = sol::property(&Engine::getPreferedCanvasWidth);
    engine["preferedCanvasHeight"] = sol::property(&Engine::getPreferedCanvasHeight);
    engine["calculateCanvas"] = &Engine::calculateCanvas;
    engine["viewRect"] = sol::property(&Engine::getViewRect);
    engine["scalingMode"] = sol::property(&Engine::getScalingMode, &Engine::setScalingMode);
    engine["textureStrategy"] = sol::property(&Engine::getTextureStrategy, &Engine::setTextureStrategy);
    engine["callMouseInTouchEvent"] = sol::property(&Engine::isCallMouseInTouchEvent, &Engine::setCallMouseInTouchEvent);
    engine["callTouchInMouseEvent"] = sol::property(&Engine::isCallTouchInMouseEvent, &Engine::setCallTouchInMouseEvent);
    engine["useDegrees"] = sol::property(&Engine::isUseDegrees, &Engine::setUseDegrees);
    engine["defaultNearestScaleTexture"] = sol::property(&Engine::isDefaultNearestScaleTexture, &Engine::setDefaultNearestScaleTexture);
    engine["defaultResampleToPOTTexture"] = sol::property(&Engine::isDefaultResampleToPOTTexture, &Engine::setDefaultResampleToPOTTexture);
    engine["automaticTransparency"] = sol::property(&Engine::isAutomaticTransparency, &Engine::setAutomaticTransparency);
    engine["setAutomaticFlipY"] = sol::property(&Engine::isAutomaticFlipY, &Engine::setAutomaticFlipY);
    engine["allowEventsOutCanvas"] = sol::property(&Engine::isAllowEventsOutCanvas, &Engine::setAllowEventsOutCanvas);
    engine["fixedTimeSceneUpdate"] = sol::property(&Engine::isFixedTimeSceneUpdate, &Engine::setFixedTimeSceneUpdate);
    engine["updateTime"] = sol::property(&Engine::getUpdateTime, &Engine::setUpdateTime);
    engine["sceneUpdateTime"] = sol::property(&Engine::getSceneUpdateTime);
    engine["platform"] = sol::property(&Engine::getPlatform);
    engine["graphicBackend"] = sol::property(&Engine::getGraphicBackend);
    engine["openGL"] = sol::property(&Engine::isOpenGL);
    engine["framerate"] = sol::property(&Engine::getFramerate);
    engine["deltatime"] = sol::property(&Engine::getDeltatime);
    engine["onViewLoaded"] = sol::property([] () { return &Engine::onViewLoaded; }, [] (sol::protected_function func) { Engine::onViewLoaded.add("luaFunction", func);});
    engine["onViewChanged"] = sol::property([] () { return &Engine::onViewChanged; }, [] (sol::protected_function func) { Engine::onViewChanged.add("luaFunction", func);});
    engine["onDraw"] = sol::property([] () { return &Engine::onDraw; }, [] (sol::protected_function func) { Engine::onDraw.add("luaFunction", func);});
    engine["onUpdate"] = sol::property([] () { return &Engine::onUpdate; }, [] (sol::protected_function func) { Engine::onUpdate.add("luaFunction", func);});
    engine["onShutdown"] = sol::property([] () { return &Engine::onShutdown; }, [] (sol::protected_function func) { Engine::onShutdown.add("luaFunction", func);});
    engine["onTouchStart"] = sol::property([] () { return &Engine::onTouchStart; }, [] (sol::protected_function func) { Engine::onTouchStart.add("luaFunction", func);});
    engine["onTouchEnd"] = sol::property([] () { return &Engine::onTouchEnd; }, [] (sol::protected_function func) { Engine::onTouchEnd.add("luaFunction", func);});
    engine["onTouchMove"] = sol::property([] () { return &Engine::onTouchMove; }, [] (sol::protected_function func) { Engine::onTouchMove.add("luaFunction", func);});
    engine["onTouchCancel"] = sol::property([] () { return &Engine::onTouchCancel; }, [] (sol::protected_function func) { Engine::onTouchCancel.add("luaFunction", func);});
    engine["onMouseDown"] = sol::property([] () { return &Engine::onMouseDown; }, [] (sol::protected_function func) { Engine::onMouseDown.add("luaFunction", func);});
    engine["onMouseUp"] = sol::property([] () { return &Engine::onMouseUp; }, [] (sol::protected_function func) { Engine::onMouseUp.add("luaFunction", func);});
    engine["onMouseScroll"] = sol::property([] () { return &Engine::onMouseScroll; }, [] (sol::protected_function func) { Engine::onMouseScroll.add("luaFunction", func);});
    engine["onMouseMove"] = sol::property([] () { return &Engine::onMouseMove; }, [] (sol::protected_function func) { Engine::onMouseMove.add("luaFunction", func);});
    engine["onMouseEnter"] = sol::property([] () { return &Engine::onMouseEnter; }, [] (sol::protected_function func) { Engine::onMouseEnter.add("luaFunction", func);});
    engine["onMouseLeave"] = sol::property([] () { return &Engine::onMouseLeave; }, [] (sol::protected_function func) { Engine::onMouseLeave.add("luaFunction", func);});
    engine["onKeyDown"] = sol::property([] () { return &Engine::onKeyDown; }, [] (sol::protected_function func) { Engine::onKeyDown.add("luaFunction", func);});
    engine["onKeyUp"] = sol::property([] () { return &Engine::onKeyUp; }, [] (sol::protected_function func) { Engine::onKeyUp.add("luaFunction", func);});
    engine["onCharInput"] = sol::property([] () { return &Engine::onCharInput; }, [] (sol::protected_function func) { Engine::onCharInput.add("luaFunction", func);});

    // sol::meta_function::call and other metafunctions are automatically generated: https://sol2.readthedocs.io/en/latest/api/usertype.html
    lua.new_usertype<FunctionSubscribe<void()>>("FunctionSubscribe_V",
        sol::call_constructor, sol::default_constructor,
        "call", &FunctionSubscribe<void()>::call,
        "add", (bool (FunctionSubscribe<void()>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void()>::add
        );

    lua.new_usertype<FunctionSubscribe<void(int,float,float)>>("FunctionSubscribe_V_IFF",
        sol::call_constructor, sol::default_constructor,
        "call", &FunctionSubscribe<void(int,float,float)>::call,
        "add", (bool (FunctionSubscribe<void(int,float,float)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,float,float)>::add
        );

    lua.new_usertype<FunctionSubscribe<void(int,float,float,int)>>("FunctionSubscribe_V_IFFI",
        sol::call_constructor, sol::default_constructor,
        "call", &FunctionSubscribe<void(int,float,float,int)>::call,
        "add", (bool (FunctionSubscribe<void(int,float,float,int)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,float,float,int)>::add
        );

    lua.new_usertype<FunctionSubscribe<void(int,bool,int)>>("FunctionSubscribe_V_IBI",
        sol::call_constructor, sol::default_constructor,
        "call", &FunctionSubscribe<void(int,bool,int)>::call,
        "add", (bool (FunctionSubscribe<void(int,bool,int)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,bool,int)>::add
        );

    lua.new_usertype<FunctionSubscribe<void(wchar_t)>>("FunctionSubscribe_V_W",
        sol::call_constructor, sol::default_constructor,
        "call", &FunctionSubscribe<void(wchar_t)>::call,
        "add", (bool (FunctionSubscribe<void(wchar_t)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(wchar_t)>::add
        );

    auto texturerender = lua.new_usertype<TextureRender>("TextureRender",  
            sol::call_constructor, sol::default_constructor);

    texturerender["createTexture"] = &TextureRender::createTexture;
    texturerender["createFramebufferTexture"] = &TextureRender::createFramebufferTexture;
    texturerender["destroyTexture"] = &TextureRender::destroyTexture;

    auto framebufferrender = lua.new_usertype<FramebufferRender>("FramebufferRender",  
            sol::call_constructor, sol::default_constructor);

    framebufferrender["createFramebuffer"] = &FramebufferRender::createFramebuffer;
    framebufferrender["destroyFramebuffer"] = &FramebufferRender::destroyFramebuffer;
    framebufferrender["isCreated"] = &FramebufferRender::isCreated;
    framebufferrender["colorTexture"] = sol::property(&FramebufferRender::getColorTexture);

    auto scene = lua.new_usertype<Scene>("Scene",
	     sol::call_constructor, sol::default_constructor);

    scene["load"] = &Scene::load;
    scene["destroy"] = &Scene::destroy;
    scene["draw"] = &Scene::draw;
    scene["update"] = &Scene::update;
    scene["camera"] = sol::property(&Scene::getCamera, &Scene::setCamera);
    scene["setCamera"] = &Scene::setCamera;
    scene["getCamera"] = &Scene::getCamera;
    scene["mainScene"] = sol::property(&Scene::isMainScene, &Scene::setMainScene);
    scene["setMainScene"] = &Scene::setMainScene;
    scene["isMainScene"] = &Scene::isMainScene;
    scene["backgroundColor"] = sol::property( &Scene::getBackgroundColor, sol::resolve<void(Vector4)>(&Scene::setBackgroundColor));
    scene["setBackgroundColor"] = sol::overload( sol::resolve<void(float, float, float)>(&Scene::setBackgroundColor), sol::resolve<void(Vector4)>(&Scene::setBackgroundColor) );
    scene["getBackgroundColor"] = &Scene::getBackgroundColor;
    scene["shadowsPCF"] = sol::property(&Scene::isShadowsPCF, &Scene::setShadowsPCF);
    scene["setShadowsPCF"] = &Scene::setShadowsPCF;
    scene["isShadowsPCF"] = &Scene::isShadowsPCF;
    scene["fogEnabled"] = sol::property(&Scene::isFogEnabled, &Scene::setFogEnabled);
    scene["setFogEnabled"] = &Scene::setFogEnabled;
    scene["isFogEnabled"] = &Scene::isFogEnabled;
    scene["fog"] = sol::property(&Scene::getFog);
    scene["getFog"] = &Scene::getFog;
    scene["ambientLightColor"] = sol::property(&Scene::getAmbientLightColor, sol::resolve<void(Vector3)>(&Scene::setAmbientLight));
    scene["setAmbientLight"] = sol::overload( sol::resolve<void(float)>(&Scene::setAmbientLight), sol::resolve<void(Vector3)>(&Scene::setAmbientLight), sol::resolve<void(float, Vector3)>(&Scene::setAmbientLight) );
    scene["getAmbientLightColor"] = &Scene::getAmbientLightColor;
    scene["ambientLightFactor"] = sol::property(&Scene::getAmbientLightFactor, sol::resolve<void(float)>(&Scene::setAmbientLight));
    scene["getAmbientLightFactor"] = &Scene::getAmbientLightFactor;
    scene["sceneAmbientLightEnabled"] = sol::property(&Scene::isSceneAmbientLightEnabled, &Scene::setSceneAmbientLightEnabled);
    scene["isSceneAmbientLightEnabled"] = &Scene::isSceneAmbientLightEnabled;
    scene["setSceneAmbientLightEnabled"] = &Scene::setSceneAmbientLightEnabled;
    scene["renderToTexture"] = sol::property(&Scene::isRenderToTexture, &Scene::setRenderToTexture);
    scene["setRenderToTexture"] = &Scene::setRenderToTexture;
    scene["isRenderToTexture"] = &Scene::isRenderToTexture;
    scene["getFramebuffer"] = &Scene::getFramebuffer;
    scene["setFramebufferSize"] = &Scene::setFramebufferSize;
    scene["getFramebufferWidth"] = &Scene::getFramebufferWidth;
    scene["getFramebufferHeight"] = &Scene::getFramebufferHeight;
    scene["updateCameraSize"] = &Scene::updateCameraSize;
    scene["findBranchLastIndex"] = &Scene::findBranchLastIndex;
    scene["createEntity"] = &Scene::createEntity;
    scene["destroyEntity"] = &Scene::destroyEntity;
    scene["addEntityChild"] = &Scene::addEntityChild;
    scene["moveChildToFirst"] = &Scene::moveChildToFirst;
    scene["moveChildUp"] = &Scene::moveChildUp;
    scene["moveChildDown"] = &Scene::moveChildDown;
    scene["moveChildToLast"] = &Scene::moveChildToLast;


    auto log = lua.new_usertype<Log>("Log",
        sol::no_constructor);

    log["Print"] = &Log::Print;
    log["Verbose"] = &Log::Verbose;
    log["Debug"] = &Log::Debug;
    log["Warn"] = &Log::Warn;
    log["Error"] = &Log::Error;


    auto touch = lua.new_usertype<Touch>("Touch",
        sol::no_constructor);

    touch["pointer"] = &Touch::pointer;
    touch["position"] = &Touch::position;

    auto input = lua.new_usertype<Input>("Input",
        sol::no_constructor);

    input["MODIFIER_SHIFT"] = sol::var(S_MODIFIER_SHIFT);
    input["MODIFIER_CONTROL"] = sol::var(S_MODIFIER_CONTROL);
    input["MODIFIER_ALT"] = sol::var(S_MODIFIER_ALT);
    input["MODIFIER_SUPER"] = sol::var(S_MODIFIER_SUPER);
    input["MODIFIER_CAPS_LOCK"] = sol::var(S_MODIFIER_CAPS_LOCK);
    input["MODIFIER_NUM_LOCK"] = sol::var(S_MODIFIER_NUM_LOCK);
    input["KEY_UNKNOWN"] = sol::var(S_KEY_UNKNOWN);
    input["KEY_SPACE"] = sol::var(S_KEY_SPACE);
    input["KEY_APOSTROPHE"] = sol::var(S_KEY_APOSTROPHE);
    input["KEY_COMMA"] = sol::var(S_KEY_COMMA);
    input["KEY_MINUS"] = sol::var(S_KEY_MINUS);
    input["KEY_PERIOD"] = sol::var(S_KEY_PERIOD);
    input["KEY_SLASH"] = sol::var(S_KEY_SLASH);
    input["KEY_0"] = sol::var(S_KEY_0);
    input["KEY_1"] = sol::var(S_KEY_1);
    input["KEY_2"] = sol::var(S_KEY_2);
    input["KEY_3"] = sol::var(S_KEY_3);
    input["KEY_4"] = sol::var(S_KEY_4);
    input["KEY_5"] = sol::var(S_KEY_5);
    input["KEY_6"] = sol::var(S_KEY_6);
    input["KEY_7"] = sol::var(S_KEY_7);
    input["KEY_8"] = sol::var(S_KEY_8);
    input["KEY_9"] = sol::var(S_KEY_9);
    input["KEY_SEMICOLON"] = sol::var(S_KEY_SEMICOLON);
    input["KEY_EQUAL"] = sol::var(S_KEY_EQUAL);
    input["KEY_A"] = sol::var(S_KEY_A);
    input["KEY_B"] = sol::var(S_KEY_B);
    input["KEY_C"] = sol::var(S_KEY_C);
    input["KEY_D"] = sol::var(S_KEY_D);
    input["KEY_E"] = sol::var(S_KEY_E);
    input["KEY_F"] = sol::var(S_KEY_F);
    input["KEY_G"] = sol::var(S_KEY_G);
    input["KEY_H"] = sol::var(S_KEY_H);
    input["KEY_I"] = sol::var(S_KEY_I);
    input["KEY_J"] = sol::var(S_KEY_J);
    input["KEY_K"] = sol::var(S_KEY_K);
    input["KEY_L"] = sol::var(S_KEY_L);
    input["KEY_M"] = sol::var(S_KEY_M);
    input["KEY_N"] = sol::var(S_KEY_N);
    input["KEY_O"] = sol::var(S_KEY_O);
    input["KEY_P"] = sol::var(S_KEY_P);
    input["KEY_Q"] = sol::var(S_KEY_Q);
    input["KEY_R"] = sol::var(S_KEY_R);
    input["KEY_S"] = sol::var(S_KEY_S);
    input["KEY_T"] = sol::var(S_KEY_T);
    input["KEY_U"] = sol::var(S_KEY_U);
    input["KEY_V"] = sol::var(S_KEY_V);
    input["KEY_W"] = sol::var(S_KEY_W);
    input["KEY_X"] = sol::var(S_KEY_X);
    input["KEY_Y"] = sol::var(S_KEY_Y);
    input["KEY_Z"] = sol::var(S_KEY_Z);
    input["KEY_LEFT_BRACKET"] = sol::var(S_KEY_LEFT_BRACKET);
    input["KEY_BACKSLASH"] = sol::var(S_KEY_BACKSLASH);
    input["KEY_RIGHT_BRACKET"] = sol::var(S_KEY_RIGHT_BRACKET);
    input["KEY_GRAVE_ACCENT"] = sol::var(S_KEY_GRAVE_ACCENT);
    input["KEY_WORLD_1"] = sol::var(S_KEY_WORLD_1);
    input["KEY_WORLD_2"] = sol::var(S_KEY_WORLD_2);
    input["KEY_ESCAPE"] = sol::var(S_KEY_ESCAPE);
    input["KEY_ENTER"] = sol::var(S_KEY_ENTER);
    input["KEY_TAB"] = sol::var(S_KEY_TAB);
    input["KEY_BACKSPACE"] = sol::var(S_KEY_BACKSPACE);
    input["KEY_INSERT"] = sol::var(S_KEY_INSERT);
    input["KEY_DELETE"] = sol::var(S_KEY_DELETE);
    input["KEY_RIGHT"] = sol::var(S_KEY_RIGHT);
    input["KEY_LEFT"] = sol::var(S_KEY_LEFT);
    input["KEY_DOWN"] = sol::var(S_KEY_DOWN);
    input["KEY_UP"] = sol::var(S_KEY_UP);
    input["KEY_PAGE_UP"] = sol::var(S_KEY_PAGE_UP);
    input["KEY_PAGE_DOWN"] = sol::var(S_KEY_PAGE_DOWN);
    input["KEY_HOME"] = sol::var(S_KEY_HOME);
    input["KEY_END"] = sol::var(S_KEY_END);
    input["KEY_CAPS_LOCK"] = sol::var(S_KEY_CAPS_LOCK);
    input["KEY_SCROLL_LOCK"] = sol::var(S_KEY_SCROLL_LOCK);
    input["KEY_NUM_LOCK"] = sol::var(S_KEY_NUM_LOCK);
    input["KEY_PRINT_SCREEN"] = sol::var(S_KEY_PRINT_SCREEN);
    input["KEY_PAUSE"] = sol::var(S_KEY_PAUSE);
    input["KEY_F1"] = sol::var(S_KEY_F1);
    input["KEY_F2"] = sol::var(S_KEY_F2);
    input["KEY_F3"] = sol::var(S_KEY_F3);
    input["KEY_F4"] = sol::var(S_KEY_F4);
    input["KEY_F5"] = sol::var(S_KEY_F5);
    input["KEY_F6"] = sol::var(S_KEY_F6);
    input["KEY_F7"] = sol::var(S_KEY_F7);
    input["KEY_F8"] = sol::var(S_KEY_F8);
    input["KEY_F9"] = sol::var(S_KEY_F9);
    input["KEY_F10"] = sol::var(S_KEY_F10);
    input["KEY_F11"] = sol::var(S_KEY_F11);
    input["KEY_F12"] = sol::var(S_KEY_F12);
    input["KEY_F13"] = sol::var(S_KEY_F13);
    input["KEY_F14"] = sol::var(S_KEY_F14);
    input["KEY_F15"] = sol::var(S_KEY_F15);
    input["KEY_F16"] = sol::var(S_KEY_F16);
    input["KEY_F17"] = sol::var(S_KEY_F17);
    input["KEY_F18"] = sol::var(S_KEY_F18);
    input["KEY_F19"] = sol::var(S_KEY_F19);
    input["KEY_F20"] = sol::var(S_KEY_F20);
    input["KEY_F21"] = sol::var(S_KEY_F21);
    input["KEY_F22"] = sol::var(S_KEY_F22);
    input["KEY_F23"] = sol::var(S_KEY_F23);
    input["KEY_F24"] = sol::var(S_KEY_F24);
    input["KEY_F25"] = sol::var(S_KEY_F25);
    input["KEY_KP_0"] = sol::var(S_KEY_KP_0);
    input["KEY_KP_1"] = sol::var(S_KEY_KP_1);
    input["KEY_KP_2"] = sol::var(S_KEY_KP_2);
    input["KEY_KP_3"] = sol::var(S_KEY_KP_3);
    input["KEY_KP_4"] = sol::var(S_KEY_KP_4);
    input["KEY_KP_5"] = sol::var(S_KEY_KP_5);
    input["KEY_KP_6"] = sol::var(S_KEY_KP_6);
    input["KEY_KP_7"] = sol::var(S_KEY_KP_7);
    input["KEY_KP_8"] = sol::var(S_KEY_KP_8);
    input["KEY_KP_9"] = sol::var(S_KEY_KP_9);
    input["KEY_KP_DECIMAL"] = sol::var(S_KEY_KP_DECIMAL);
    input["KEY_KP_DIVIDE"] = sol::var(S_KEY_KP_DIVIDE);
    input["KEY_KP_MULTIPLY"] = sol::var(S_KEY_KP_MULTIPLY);
    input["KEY_KP_SUBTRACT"] = sol::var(S_KEY_KP_SUBTRACT);
    input["KEY_KP_ADD"] = sol::var(S_KEY_KP_ADD);
    input["KEY_KP_ENTER"] = sol::var(S_KEY_KP_ENTER);
    input["KEY_KP_EQUAL"] = sol::var(S_KEY_KP_EQUAL);
    input["KEY_LEFT_SHIFT"] = sol::var(S_KEY_LEFT_SHIFT);
    input["KEY_LEFT_CONTROL"] = sol::var(S_KEY_LEFT_CONTROL);
    input["KEY_LEFT_ALT"] = sol::var(S_KEY_LEFT_ALT);
    input["KEY_LEFT_SUPER"] = sol::var(S_KEY_LEFT_SUPER);
    input["KEY_RIGHT_SHIFT"] = sol::var(S_KEY_RIGHT_SHIFT);
    input["KEY_RIGHT_CONTROL"] = sol::var(S_KEY_RIGHT_CONTROL);
    input["KEY_RIGHT_ALT"] = sol::var(S_KEY_RIGHT_ALT);
    input["KEY_RIGHT_SUPER"] = sol::var(S_KEY_RIGHT_SUPER);
    input["KEY_MENU"] = sol::var(S_KEY_MENU);
    input["KEY_LAST"] = sol::var(S_KEY_LAST);

    input["isKeyPressed"] = &Input::isKeyPressed;
    input["isMousePressed"] = &Input::isMousePressed;
    input["isTouch"] = &Input::isTouch;
    input["isMouseEntered"] = &Input::isMouseEntered;
    input["getMousePosition"] = &Input::getMousePosition;
    input["getMouseScroll"] = &Input::getMouseScroll;
    input["getTouchPosition"] = &Input::getTouchPosition;
    input["getTouches"] = &Input::getTouches;
    input["numTouches"] = &Input::numTouches;
    input["getModifiers"] = &Input::getModifiers;
    input["findTouchIndex"] = &Input::findTouchIndex;
*/
#endif //DISABLE_LUA_BINDINGS
}