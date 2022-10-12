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

using namespace Supernova;

namespace luabridge
{
    template<> struct Stack<Scaling> : EnumWrapper<Scaling>{};
    template<> struct Stack<Platform> : EnumWrapper<Platform>{};
    template<> struct Stack<GraphicBackend> : EnumWrapper<GraphicBackend>{};
    template<> struct Stack<TextureStrategy> : EnumWrapper<TextureStrategy>{};
    template<> struct Stack<TextureType> : EnumWrapper<TextureType>{};
    template<> struct Stack<ColorFormat> : EnumWrapper<ColorFormat>{};

    template <>
    struct Stack <Touch>
    {
        static Result push(lua_State* L, Touch touch)
        {
            lua_newtable(L);

            lua_pushinteger(L, touch.pointer);
            lua_setfield(L, -2, "pointer");

            if (!luabridge::push(L, touch.position))
                return makeErrorCode(ErrorCode::LuaStackOverflow);
            lua_setfield(L, -2, "position");

            return {};
        }

        static TypeResult<Touch> get(lua_State* L, int index)
        {
            lua_getfield(L, index, "pointer");
            if (lua_type(L, -1) != LUA_TNUMBER)
                return makeErrorCode(ErrorCode::InvalidTypeCast);
            if (! is_integral_representable_by<int>(L, -1))
                return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            int pointer = lua_tointeger(L, -1);

            lua_getfield(L, index, "position");
            auto result = luabridge::get<Vector2>(L, -1);
            if (! result)
                return result.error();
            Vector2 position = *result;

            Touch touch = {pointer, position};
            return touch;
        }

        static bool isInstance (lua_State* L, int index)
        {
            if (lua_type (L, index) != LUA_TTABLE)
                return false;
            if (lua_getfield(L, index, "pointer") != LUA_TNUMBER)
                return false;
            if (!is_integral_representable_by<int>(L, -1))
                return false;
            if (lua_getfield(L, index, "position") != LUA_TUSERDATA)
                return false;
            if (!Stack<Vector2>::isInstance(L, -1))
                return false;

            return true;
        }
    };
}

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
        .addStaticProperty("onShutdown", [] () { return &Engine::onShutdown; }, [] (lua_State* L) { Engine::onShutdown = L; })
        .addStaticProperty("onTouchStart", [] () { return &Engine::onTouchStart; }, [] (lua_State* L) { Engine::onTouchStart = L; })
        .addStaticProperty("onTouchEnd", [] () { return &Engine::onTouchEnd; }, [] (lua_State* L) { Engine::onTouchEnd = L; })
        .addStaticProperty("onTouchMove", [] () { return &Engine::onTouchMove; }, [] (lua_State* L) { Engine::onTouchMove = L; })
        .addStaticProperty("onTouchCancel", [] () { return &Engine::onTouchCancel; }, [] (lua_State* L) { Engine::onTouchCancel = L; })
        .addStaticProperty("onMouseDown", [] () { return &Engine::onMouseDown; }, [] (lua_State* L) { Engine::onMouseDown = L; })
        .addStaticProperty("onMouseUp", [] () { return &Engine::onMouseUp; }, [] (lua_State* L) { Engine::onMouseUp = L; })
        .addStaticProperty("onMouseScroll", [] () { return &Engine::onMouseScroll; }, [] (lua_State* L) { Engine::onMouseScroll = L; })
        .addStaticProperty("onMouseMove", [] () { return &Engine::onMouseMove; }, [] (lua_State* L) { Engine::onMouseMove = L; })
        .addStaticProperty("onMouseEnter", [] () { return &Engine::onMouseEnter; }, [] (lua_State* L) { Engine::onMouseEnter = L; })
        .addStaticProperty("onMouseLeave", [] () { return &Engine::onMouseLeave; }, [] (lua_State* L) { Engine::onMouseLeave = L; })
        .addStaticProperty("onKeyDown", [] () { return &Engine::onKeyDown; }, [] (lua_State* L) { Engine::onKeyDown = L; })
        .addStaticProperty("onKeyUp", [] () { return &Engine::onKeyUp; }, [] (lua_State* L) { Engine::onKeyUp = L; })
        .addStaticProperty("onCharInput", [] () { return &Engine::onCharInput; }, [] (lua_State* L) { Engine::onCharInput = L; })

        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FunctionSubscribe<void()>>("FunctionSubscribe_V")
        .addFunction("__call", &FunctionSubscribe<void()>::call)
        .addFunction("call", &FunctionSubscribe<void()>::call)
        .addFunction("add", (bool (FunctionSubscribe<void()>::*)(const std::string&, lua_State*))&FunctionSubscribe<void()>::add)
        .endClass();

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

    luabridge::getGlobalNamespace(L)
        .beginClass<TextureRender>("TextureRender")
        .addConstructor <void (*) (void)> ()
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<FramebufferRender>("FramebufferRender")
        .addConstructor <void (*) (void)> ()
        .addFunction("isCreated", &FramebufferRender::isCreated)
        .addFunction("getColorTexture", &FramebufferRender::getColorTexture)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Scene>("Scene")
        .addConstructor <void (*) (void)> ()
        .addFunction("load", &Scene::load)
        .addFunction("destroy", &Scene::destroy)
        .addFunction("draw", &Scene::draw)
        .addFunction("update", &Scene::update)
        .addProperty("camera", &Scene::getCamera, &Scene::setCamera)
        .addProperty("mainScene", &Scene::isMainScene, &Scene::setMainScene)
        .addProperty("backgroundColor", &Scene::getBackgroundColor, (void (Scene::*)(Vector4))&Scene::setBackgroundColor)
        .addFunction("setBackgroundColor", (void (Scene::*)(float, float, float))&Scene::setBackgroundColor)
        .addProperty("shadowsPCF", &Scene::isShadowsPCF, &Scene::setShadowsPCF)
        .addProperty("fogEnabled", &Scene::isFogEnabled, &Scene::setFogEnabled)
        .addFunction("getFog", &Scene::getFog)
        .addProperty("ambientLightColor", &Scene::getAmbientLightColor, (void (Scene::*)(Vector3))&Scene::setAmbientLight)
        .addProperty("ambientLightFactor", &Scene::getAmbientLightFactor, (void (Scene::*)(float))&Scene::setAmbientLight)
        .addFunction("setAmbientLight", (void (Scene::*)(float, Vector3))&Scene::setAmbientLight)
        .addProperty("sceneAmbientLightEnabled", &Scene::isSceneAmbientLightEnabled, &Scene::setSceneAmbientLightEnabled)
        .addProperty("renderToTexture", &Scene::isRenderToTexture, &Scene::setRenderToTexture)
        .addFunction("getFramebuffer", &Scene::getFramebuffer)
        .addFunction("setFramebufferSize", &Scene::setFramebufferSize)
        .addFunction("getFramebufferWidth", &Scene::getFramebufferWidth)
        .addFunction("getFramebufferHeight", &Scene::getFramebufferHeight)
        .addFunction("updateCameraSize", &Scene::updateCameraSize)
        .addFunction("findBranchLastIndex", &Scene::findBranchLastIndex)
        .addFunction("createEntity", &Scene::createEntity)
        .addFunction("destroyEntity", &Scene::destroyEntity)
        .addFunction("addEntityChild", &Scene::addEntityChild)
        .addFunction("moveChildToFirst", &Scene::moveChildToFirst)
        .addFunction("moveChildUp", &Scene::moveChildUp)
        .addFunction("moveChildDown", &Scene::moveChildDown)
        .addFunction("moveChildToLast", &Scene::moveChildToLast)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Log>("Log")
        .addStaticFunction("Print", [] (std::string text) { Log::Print(text.c_str()); })
        .addStaticFunction("Verbose", [] (std::string text) { Log::Verbose(text.c_str()); })
        .addStaticFunction("Debug", [] (std::string text) { Log::Debug(text.c_str()); })
        .addStaticFunction("Warn", [] (std::string text) { Log::Warn(text.c_str()); })
        .addStaticFunction("Error", [] (std::string text) { Log::Error(text.c_str()); })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Input>("Input")
        .addStaticProperty("MODIFIER_SHIFT", [] () -> int { return S_MODIFIER_SHIFT; })
        .addStaticProperty("MODIFIER_CONTROL", [] () -> int { return S_MODIFIER_CONTROL; })
        .addStaticProperty("MODIFIER_ALT", [] () -> int { return S_MODIFIER_ALT; })
        .addStaticProperty("MODIFIER_SUPER", [] () -> int { return S_MODIFIER_SUPER; })
        .addStaticProperty("MODIFIER_CAPS_LOCK", [] () -> int { return S_MODIFIER_CAPS_LOCK; })
        .addStaticProperty("MODIFIER_NUM_LOCK", [] () -> int { return S_MODIFIER_NUM_LOCK; })
        .addStaticProperty("KEY_UNKNOWN", [] () -> int { return S_KEY_UNKNOWN; })
        .addStaticProperty("KEY_SPACE", [] () -> int { return S_KEY_SPACE; })
        .addStaticProperty("KEY_APOSTROPHE", [] () -> int { return S_KEY_APOSTROPHE; })
        .addStaticProperty("KEY_COMMA", [] () -> int { return S_KEY_COMMA; })
        .addStaticProperty("KEY_MINUS", [] () -> int { return S_KEY_MINUS; })
        .addStaticProperty("KEY_PERIOD", [] () -> int { return S_KEY_PERIOD; })
        .addStaticProperty("KEY_SLASH", [] () -> int { return S_KEY_SLASH; })
        .addStaticProperty("KEY_0", [] () -> int { return S_KEY_0; })
        .addStaticProperty("KEY_1", [] () -> int { return S_KEY_1; })
        .addStaticProperty("KEY_2", [] () -> int { return S_KEY_2; })
        .addStaticProperty("KEY_3", [] () -> int { return S_KEY_3; })
        .addStaticProperty("KEY_4", [] () -> int { return S_KEY_4; })
        .addStaticProperty("KEY_5", [] () -> int { return S_KEY_5; })
        .addStaticProperty("KEY_6", [] () -> int { return S_KEY_6; })
        .addStaticProperty("KEY_7", [] () -> int { return S_KEY_7; })
        .addStaticProperty("KEY_8", [] () -> int { return S_KEY_8; })
        .addStaticProperty("KEY_9", [] () -> int { return S_KEY_9; })
        .addStaticProperty("KEY_SEMICOLON", [] () -> int { return S_KEY_SEMICOLON; })
        .addStaticProperty("KEY_EQUAL", [] () -> int { return S_KEY_EQUAL; })
        .addStaticProperty("KEY_A", [] () -> int { return S_KEY_A; })
        .addStaticProperty("KEY_B", [] () -> int { return S_KEY_B; })
        .addStaticProperty("KEY_C", [] () -> int { return S_KEY_C; })
        .addStaticProperty("KEY_D", [] () -> int { return S_KEY_D; })
        .addStaticProperty("KEY_E", [] () -> int { return S_KEY_E; })
        .addStaticProperty("KEY_F", [] () -> int { return S_KEY_F; })
        .addStaticProperty("KEY_G", [] () -> int { return S_KEY_G; })
        .addStaticProperty("KEY_H", [] () -> int { return S_KEY_H; })
        .addStaticProperty("KEY_I", [] () -> int { return S_KEY_I; })
        .addStaticProperty("KEY_J", [] () -> int { return S_KEY_J; })
        .addStaticProperty("KEY_K", [] () -> int { return S_KEY_K; })
        .addStaticProperty("KEY_L", [] () -> int { return S_KEY_L; })
        .addStaticProperty("KEY_M", [] () -> int { return S_KEY_M; })
        .addStaticProperty("KEY_N", [] () -> int { return S_KEY_N; })
        .addStaticProperty("KEY_O", [] () -> int { return S_KEY_O; })
        .addStaticProperty("KEY_P", [] () -> int { return S_KEY_P; })
        .addStaticProperty("KEY_Q", [] () -> int { return S_KEY_Q; })
        .addStaticProperty("KEY_R", [] () -> int { return S_KEY_R; })
        .addStaticProperty("KEY_S", [] () -> int { return S_KEY_S; })
        .addStaticProperty("KEY_T", [] () -> int { return S_KEY_T; })
        .addStaticProperty("KEY_U", [] () -> int { return S_KEY_U; })
        .addStaticProperty("KEY_V", [] () -> int { return S_KEY_V; })
        .addStaticProperty("KEY_W", [] () -> int { return S_KEY_W; })
        .addStaticProperty("KEY_X", [] () -> int { return S_KEY_X; })
        .addStaticProperty("KEY_Y", [] () -> int { return S_KEY_Y; })
        .addStaticProperty("KEY_Z", [] () -> int { return S_KEY_Z; })
        .addStaticProperty("KEY_LEFT_BRACKET", [] () -> int { return S_KEY_LEFT_BRACKET; })
        .addStaticProperty("KEY_BACKSLASH", [] () -> int { return S_KEY_BACKSLASH; })
        .addStaticProperty("KEY_RIGHT_BRACKET", [] () -> int { return S_KEY_RIGHT_BRACKET; })
        .addStaticProperty("KEY_GRAVE_ACCENT", [] () -> int { return S_KEY_GRAVE_ACCENT; })
        .addStaticProperty("KEY_WORLD_1", [] () -> int { return S_KEY_WORLD_1; })
        .addStaticProperty("KEY_WORLD_2", [] () -> int { return S_KEY_WORLD_2; })
        .addStaticProperty("KEY_ESCAPE", [] () -> int { return S_KEY_ESCAPE; })
        .addStaticProperty("KEY_ENTER", [] () -> int { return S_KEY_ENTER; })
        .addStaticProperty("KEY_TAB", [] () -> int { return S_KEY_TAB; })
        .addStaticProperty("KEY_BACKSPACE", [] () -> int { return S_KEY_BACKSPACE; })
        .addStaticProperty("KEY_INSERT", [] () -> int { return S_KEY_INSERT; })
        .addStaticProperty("KEY_DELETE", [] () -> int { return S_KEY_DELETE; })
        .addStaticProperty("KEY_RIGHT", [] () -> int { return S_KEY_RIGHT; })
        .addStaticProperty("KEY_LEFT", [] () -> int { return S_KEY_LEFT; })
        .addStaticProperty("KEY_DOWN", [] () -> int { return S_KEY_DOWN; })
        .addStaticProperty("KEY_UP", [] () -> int { return S_KEY_UP; })
        .addStaticProperty("KEY_PAGE_UP", [] () -> int { return S_KEY_PAGE_UP; })
        .addStaticProperty("KEY_PAGE_DOWN", [] () -> int { return S_KEY_PAGE_DOWN; })
        .addStaticProperty("KEY_HOME", [] () -> int { return S_KEY_HOME; })
        .addStaticProperty("KEY_END", [] () -> int { return S_KEY_END; })
        .addStaticProperty("KEY_CAPS_LOCK", [] () -> int { return S_KEY_CAPS_LOCK; })
        .addStaticProperty("KEY_SCROLL_LOCK", [] () -> int { return S_KEY_SCROLL_LOCK; })
        .addStaticProperty("KEY_NUM_LOCK", [] () -> int { return S_KEY_NUM_LOCK; })
        .addStaticProperty("KEY_PRINT_SCREEN", [] () -> int { return S_KEY_PRINT_SCREEN; })
        .addStaticProperty("KEY_PAUSE", [] () -> int { return S_KEY_PAUSE; })
        .addStaticProperty("KEY_F1", [] () -> int { return S_KEY_F1; })
        .addStaticProperty("KEY_F2", [] () -> int { return S_KEY_F2; })
        .addStaticProperty("KEY_F3", [] () -> int { return S_KEY_F3; })
        .addStaticProperty("KEY_F4", [] () -> int { return S_KEY_F4; })
        .addStaticProperty("KEY_F5", [] () -> int { return S_KEY_F5; })
        .addStaticProperty("KEY_F6", [] () -> int { return S_KEY_F6; })
        .addStaticProperty("KEY_F7", [] () -> int { return S_KEY_F7; })
        .addStaticProperty("KEY_F8", [] () -> int { return S_KEY_F8; })
        .addStaticProperty("KEY_F9", [] () -> int { return S_KEY_F9; })
        .addStaticProperty("KEY_F10", [] () -> int { return S_KEY_F10; })
        .addStaticProperty("KEY_F11", [] () -> int { return S_KEY_F11; })
        .addStaticProperty("KEY_F12", [] () -> int { return S_KEY_F12; })
        .addStaticProperty("KEY_F13", [] () -> int { return S_KEY_F13; })
        .addStaticProperty("KEY_F14", [] () -> int { return S_KEY_F14; })
        .addStaticProperty("KEY_F15", [] () -> int { return S_KEY_F15; })
        .addStaticProperty("KEY_F16", [] () -> int { return S_KEY_F16; })
        .addStaticProperty("KEY_F17", [] () -> int { return S_KEY_F17; })
        .addStaticProperty("KEY_F18", [] () -> int { return S_KEY_F18; })
        .addStaticProperty("KEY_F19", [] () -> int { return S_KEY_F19; })
        .addStaticProperty("KEY_F20", [] () -> int { return S_KEY_F20; })
        .addStaticProperty("KEY_F21", [] () -> int { return S_KEY_F21; })
        .addStaticProperty("KEY_F22", [] () -> int { return S_KEY_F22; })
        .addStaticProperty("KEY_F23", [] () -> int { return S_KEY_F23; })
        .addStaticProperty("KEY_F24", [] () -> int { return S_KEY_F24; })
        .addStaticProperty("KEY_F25", [] () -> int { return S_KEY_F25; })
        .addStaticProperty("KEY_KP_0", [] () -> int { return S_KEY_KP_0; })
        .addStaticProperty("KEY_KP_1", [] () -> int { return S_KEY_KP_1; })
        .addStaticProperty("KEY_KP_2", [] () -> int { return S_KEY_KP_2; })
        .addStaticProperty("KEY_KP_3", [] () -> int { return S_KEY_KP_3; })
        .addStaticProperty("KEY_KP_4", [] () -> int { return S_KEY_KP_4; })
        .addStaticProperty("KEY_KP_5", [] () -> int { return S_KEY_KP_5; })
        .addStaticProperty("KEY_KP_6", [] () -> int { return S_KEY_KP_6; })
        .addStaticProperty("KEY_KP_7", [] () -> int { return S_KEY_KP_7; })
        .addStaticProperty("KEY_KP_8", [] () -> int { return S_KEY_KP_8; })
        .addStaticProperty("KEY_KP_9", [] () -> int { return S_KEY_KP_9; })
        .addStaticProperty("KEY_KP_DECIMAL", [] () -> int { return S_KEY_KP_DECIMAL; })
        .addStaticProperty("KEY_KP_DIVIDE", [] () -> int { return S_KEY_KP_DIVIDE; })
        .addStaticProperty("KEY_KP_MULTIPLY", [] () -> int { return S_KEY_KP_MULTIPLY; })
        .addStaticProperty("KEY_KP_SUBTRACT", [] () -> int { return S_KEY_KP_SUBTRACT; })
        .addStaticProperty("KEY_KP_ADD", [] () -> int { return S_KEY_KP_ADD; })
        .addStaticProperty("KEY_KP_ENTER", [] () -> int { return S_KEY_KP_ENTER; })
        .addStaticProperty("KEY_KP_EQUAL", [] () -> int { return S_KEY_KP_EQUAL; })
        .addStaticProperty("KEY_LEFT_SHIFT", [] () -> int { return S_KEY_LEFT_SHIFT; })
        .addStaticProperty("KEY_LEFT_CONTROL", [] () -> int { return S_KEY_LEFT_CONTROL; })
        .addStaticProperty("KEY_LEFT_ALT", [] () -> int { return S_KEY_LEFT_ALT; })
        .addStaticProperty("KEY_LEFT_SUPER", [] () -> int { return S_KEY_LEFT_SUPER; })
        .addStaticProperty("KEY_RIGHT_SHIFT", [] () -> int { return S_KEY_RIGHT_SHIFT; })
        .addStaticProperty("KEY_RIGHT_CONTROL", [] () -> int { return S_KEY_RIGHT_CONTROL; })
        .addStaticProperty("KEY_RIGHT_ALT", [] () -> int { return S_KEY_RIGHT_ALT; })
        .addStaticProperty("KEY_RIGHT_SUPER", [] () -> int { return S_KEY_RIGHT_SUPER; })
        .addStaticProperty("KEY_MENU", [] () -> int { return S_KEY_MENU; })
        .addStaticProperty("KEY_LAST", [] () -> int { return S_KEY_LAST; })

        .addStaticFunction("isKeyPressed", &Input::isKeyPressed)
        .addStaticFunction("isMousePressed", &Input::isMousePressed)
        .addStaticFunction("isTouch", &Input::isTouch)
        .addStaticFunction("isMouseEntered", &Input::isMouseEntered)
        .addStaticFunction("getMousePosition", &Input::getMousePosition)
        .addStaticFunction("getMouseScroll", &Input::getMouseScroll)
        .addStaticFunction("getTouchPosition", &Input::getTouchPosition)
        .addStaticFunction("getTouches", &Input::getTouches)
        .addStaticFunction("numTouches", &Input::numTouches)
        .addStaticFunction("getModifiers", &Input::getModifiers)
        .addStaticFunction("findTouchIndex", &Input::findTouchIndex)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}