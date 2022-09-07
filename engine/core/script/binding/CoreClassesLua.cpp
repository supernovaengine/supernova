//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "Engine.h"
#include "Log.h"
#include "Scene.h"

using namespace Supernova;

void LuaBinding::registerCoreClasses(lua_State *L){
    sol::state_view lua(L);

    // luaL_openlibs() opened all libraries already: base, string, io, os, package, table, debug
    //lua.open_libraries(sol::lib::base);

    lua.new_enum("Scaling",
                "FITWIDTH", Scaling::FITWIDTH,
                "FITHEIGHT", Scaling::FITHEIGHT,
                "LETTERBOX", Scaling::LETTERBOX,
                "CROP", Scaling::CROP,
                "STRETCH", Scaling::STRETCH
                );

    lua.new_enum("Platform",
                "MacOS", Platform::MacOS,
                "iOS", Platform::iOS,
                "Web", Platform::Web,
                "Android", Platform::Android,
                "Linux", Platform::Linux,
                "Windows", Platform::Windows
                );

    lua.new_enum("GraphicBackend",
                "GLCORE33", GraphicBackend::GLCORE33,
                "GLES2", GraphicBackend::GLES2,
                "GLES3", GraphicBackend::GLES3,
                "D3D11", GraphicBackend::D3D11,
                "METAL", GraphicBackend::METAL,
                "WGPU", GraphicBackend::WGPU
                );

    lua.new_enum("TextureStrategy",
                "FIT", TextureStrategy::FIT,
                "RESAMPLE", TextureStrategy::RESAMPLE,
                "NONE", TextureStrategy::NONE
                );

    lua.new_enum("TextureType",
                "TEXTURE_2D", TextureType::TEXTURE_2D,
                "TEXTURE_3D", TextureType::TEXTURE_3D,
                "TEXTURE_CUBE", TextureType::TEXTURE_CUBE,
                "TEXTURE_ARRAY", TextureType::TEXTURE_ARRAY
                );

    lua.new_enum("ColorFormat",
            "RED", ColorFormat::RED,
            "RGBA", ColorFormat::RGBA
            );

    lua.new_enum("LightType",
            "DIRECTIONAL", LightType::DIRECTIONAL,
            "POINT", LightType::POINT,
            "SPOT", LightType::SPOT
            );

    auto engine = lua.new_usertype<Engine>("Engine",
            sol::call_constructor, sol::default_constructor);

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

}