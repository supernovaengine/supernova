//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "Log.h"
#include "System.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "Engine.h"
#include "Log.h"
#include "Scene.h"

#include <map>
#include <locale>
#include <vector>
#include <memory>

using namespace Supernova;



lua_State *LuaBinding::luastate;


LuaBinding::LuaBinding() {

}

LuaBinding::~LuaBinding() {

}


void LuaBinding::createLuaState(){
    LuaBinding::luastate = luaL_newstate();
}

lua_State* LuaBinding::getLuaState(){
    return luastate;
}

void LuaBinding::luaCallback(int nargs, int nresults, int msgh){
    int status = lua_pcall(LuaBinding::getLuaState(), nargs, nresults, msgh);
    if (status != 0){
        Log::Error("Lua Error: %s", lua_tostring(LuaBinding::getLuaState(), -1));
    }
}

void LuaBinding::stackDump (lua_State *L) {
    int i = lua_gettop(L);
    Log::Debug(" ----------------  Stack Dump ----------------" );
    while(  i   ) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:
                Log::Debug("%d:`%s'", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                Log::Debug("%d: %s",i,lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                Log::Debug("%d: %g",  i, lua_tonumber(L, i));
                break;
            default: Log::Debug("%d: %s", i, lua_typename(L, t)); break;
        }
        i--;
    }
    Log::Debug("--------------- Stack Dump Finished ---------------" );
}

int LuaBinding::setLuaSearcher(lua_CFunction f, bool cleanSearchers) {

    lua_State *L = LuaBinding::getLuaState();

    // Add the package loader to the package.loaders table.
    lua_getglobal(L, "package");
    if(lua_isnil(L, -1))
        return luaL_error(L, "package table does not exist.");

    lua_getfield(L, -1, "searchers");
    if(lua_isnil(L, -1))
        return luaL_error(L, "package.loaders table does not exist.");

    size_t numloaders = lua_rawlen(L, -1);

    if (cleanSearchers) {
        //remove preconfigured loaders
        for (int i = 0; i < numloaders; i++) {
            lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        //add new loader
        lua_pushcfunction(L, f);
        lua_rawseti(L, -2, 1);
    }else{
        lua_pushcfunction(L, f);
        lua_rawseti(L, -2, numloaders+1);
    }

    lua_pop(L, 1);

    return 0;
}

// Note it can be done in the same way with Sol2: https://github.com/ThePhD/sol2/issues/692
int LuaBinding::setLuaPath(const char* path) {
    lua_State *L = LuaBinding::getLuaState();

    lua_getglobal( L, "package" );
    if(lua_isnil(L, -1))
        return luaL_error(L, "package table does not exist.");

    lua_getfield( L, -1, "path" );
    if(lua_isnil(L, -1))
        return luaL_error(L, "package.path table does not exist.");

    std::string cur_path = lua_tostring( L, -1 );
    cur_path.append( ";" );
    cur_path.append( path );
    lua_pop( L, 1 );
    lua_pushstring( L, cur_path.c_str() );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );

    return 0;
}

int LuaBinding::moduleLoader(lua_State *L) {
    
    const char *filename = lua_tostring(L, 1);
    filename = luaL_gsub(L, filename, ".", std::to_string(System::instance().getDirSeparator()).c_str());
    
    std::string filepath;
    Data filedata;
    
    filepath = "lua://" + std::string("lua") + System::instance().getDirSeparator() + filename + ".lua";
    filedata.open(filepath.c_str());
    if (filedata.getMemPtr() != NULL) {
        
        luaL_loadbuffer(L, (const char *) filedata.getMemPtr(), filedata.length(),
                        filepath.c_str());
        
        return 1;
    }
    
    filepath = "lua://" + std::string("") + filename + ".lua";
    filedata.open(filepath.c_str());
    if (filedata.getMemPtr() != NULL) {
        
        luaL_loadbuffer(L, (const char *) filedata.getMemPtr(), filedata.length(),
                        filepath.c_str());
        
        return 1;
    }
    
    lua_pushstring(L, "\n\tno file in assets directory");
    
    return 1;
}

//The same msghandler of lua.c
int LuaBinding::handleLuaError(lua_State* L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
        return 1;  /* that is the message */
    else
        msg = lua_pushfstring(L, "(error object is a %s value)",
                                luaL_typename(L, 1));
    }
    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
    return 1;  /* return the traceback */
}

void LuaBinding::init(){

    lua_State *L = LuaBinding::getLuaState();
    luaL_openlibs(L);

    registerClasses(L);

    std::string luadir = std::string("lua") + System::instance().getDirSeparator();

    setLuaPath(std::string("lua://" + luadir + "?.lua").c_str());
    setLuaSearcher(moduleLoader, true);

    std::string luafile = std::string("lua://") + "main.lua";
    std::string luafile_subdir = std::string("lua://") + luadir + "main.lua";

    Data filedata;

    //First try open on root assets dir
    if (filedata.open(luafile.c_str()) != FileErrors::NO_ERROR){
        //Second try to open on lua dir
        filedata.open(luafile_subdir.c_str());
    }

    lua_pushcfunction(L, handleLuaError);
    int msgh = lua_gettop(L);

    //int luaL_dofile (lua_State *L, const char *filename);
    if (luaL_loadbuffer(L,(const char*)filedata.getMemPtr(),filedata.length(), luafile.c_str()) == 0){
        #ifndef NO_LUA_INIT
        if(lua_pcall(L, 0, LUA_MULTRET, msgh) != 0){
            Log::Error("Lua Error: %s", lua_tostring(L,-1));
            lua_close(L);
        }
        #endif
    }else{
        Log::Error("Lua Error: %s", lua_tostring(L,-1));
        lua_close(L);
    }

}

void LuaBinding::registerClasses(lua_State *L){

    //sol::state lua;
    sol::state_view lua(L);

    // luaL_openlibs() opened all libraries already: base, string, io, os, package, table, debug
    //lua.open_libraries(sol::lib::base);

    lua.new_usertype<Entity>("Entity");

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

    lua.new_usertype<Engine>("Engine",
            sol::default_constructor,
	        "setScene", &Engine::setScene,
            "getScene", &Engine::getScene,
            //"scene", sol::property(&Engine::getScene, &Engine::setScene),
            "addSceneLayer", &Engine::addSceneLayer,
            "canvasWidth", sol::property(&Engine::getCanvasWidth),
            "canvasHeight", sol::property(&Engine::getCanvasHeight),
            "setCanvasSize", &Engine::setCanvasSize,
            "preferedCanvasWidth", sol::property(&Engine::getPreferedCanvasWidth),
            "preferedCanvasHeight", sol::property(&Engine::getPreferedCanvasHeight),
            "calculateCanvas", &Engine::calculateCanvas,
            "viewRect", sol::property(&Engine::getViewRect),
            "scalingMode", sol::property(&Engine::getScalingMode, &Engine::setScalingMode),
            "textureStrategy", sol::property(&Engine::getTextureStrategy, &Engine::setTextureStrategy),
            "callMouseInTouchEvent", sol::property(&Engine::isCallMouseInTouchEvent, &Engine::setCallMouseInTouchEvent),
            "callTouchInMouseEvent", sol::property(&Engine::isCallTouchInMouseEvent, &Engine::setCallTouchInMouseEvent),
            "useDegrees", sol::property(&Engine::isUseDegrees, &Engine::setUseDegrees),
            "defaultNearestScaleTexture", sol::property(&Engine::isDefaultNearestScaleTexture, &Engine::setDefaultNearestScaleTexture),
            "defaultResampleToPOTTexture", sol::property(&Engine::isDefaultResampleToPOTTexture, &Engine::setDefaultResampleToPOTTexture),
            "automaticTransparency", sol::property(&Engine::isAutomaticTransparency, &Engine::setAutomaticTransparency),
            "setAutomaticFlipY", sol::property(&Engine::isAutomaticFlipY, &Engine::setAutomaticFlipY),
            "allowEventsOutCanvas", sol::property(&Engine::isAllowEventsOutCanvas, &Engine::setAllowEventsOutCanvas),
            "fixedTimeSceneUpdate", sol::property(&Engine::isFixedTimeSceneUpdate, &Engine::setFixedTimeSceneUpdate),
            "updateTime", sol::property(&Engine::getUpdateTime, &Engine::setUpdateTime),
            "sceneUpdateTime", sol::property(&Engine::getSceneUpdateTime),
            "platform", sol::property(&Engine::getPlatform),
            "graphicBackend", sol::property(&Engine::getGraphicBackend),
            "openGL", sol::property(&Engine::isOpenGL),
            "framerate", sol::property(&Engine::getFramerate),
            "deltatime", sol::property(&Engine::getDeltatime),
            "onViewLoaded", sol::property([] () { return &Engine::onViewLoaded; }, [] (sol::protected_function func) { Engine::onViewLoaded.add("luaFunction", func);}),
            "onViewChanged", sol::property([] () { return &Engine::onViewChanged; }, [] (sol::protected_function func) { Engine::onViewChanged.add("luaFunction", func);}),
            "onDraw", sol::property([] () { return &Engine::onDraw; }, [] (sol::protected_function func) { Engine::onDraw.add("luaFunction", func);}),
            "onUpdate", sol::property([] () { return &Engine::onUpdate; }, [] (sol::protected_function func) { Engine::onUpdate.add("luaFunction", func);}),
            "onShutdown", sol::property([] () { return &Engine::onShutdown; }, [] (sol::protected_function func) { Engine::onShutdown.add("luaFunction", func);}),
            "onTouchStart", sol::property([] () { return &Engine::onTouchStart; }, [] (sol::protected_function func) { Engine::onTouchStart.add("luaFunction", func);}),
            "onTouchEnd", sol::property([] () { return &Engine::onTouchEnd; }, [] (sol::protected_function func) { Engine::onTouchEnd.add("luaFunction", func);}),
            "onTouchMove", sol::property([] () { return &Engine::onTouchMove; }, [] (sol::protected_function func) { Engine::onTouchMove.add("luaFunction", func);}),
            "onTouchCancel", sol::property([] () { return &Engine::onTouchCancel; }, [] (sol::protected_function func) { Engine::onTouchCancel.add("luaFunction", func);}),
            "onMouseDown", sol::property([] () { return &Engine::onMouseDown; }, [] (sol::protected_function func) { Engine::onMouseDown.add("luaFunction", func);}),
            "onMouseUp", sol::property([] () { return &Engine::onMouseUp; }, [] (sol::protected_function func) { Engine::onMouseUp.add("luaFunction", func);}),
            "onMouseScroll", sol::property([] () { return &Engine::onMouseScroll; }, [] (sol::protected_function func) { Engine::onMouseScroll.add("luaFunction", func);}),
            "onMouseMove", sol::property([] () { return &Engine::onMouseMove; }, [] (sol::protected_function func) { Engine::onMouseMove.add("luaFunction", func);}),
            "onMouseEnter", sol::property([] () { return &Engine::onMouseEnter; }, [] (sol::protected_function func) { Engine::onMouseEnter.add("luaFunction", func);}),
            "onMouseLeave", sol::property([] () { return &Engine::onMouseLeave; }, [] (sol::protected_function func) { Engine::onMouseLeave.add("luaFunction", func);}),
            "onKeyDown", sol::property([] () { return &Engine::onKeyDown; }, [] (sol::protected_function func) { Engine::onKeyDown.add("luaFunction", func);}),
            "onKeyUp", sol::property([] () { return &Engine::onKeyUp; }, [] (sol::protected_function func) { Engine::onKeyUp.add("luaFunction", func);}),
            "onCharInput", sol::property([] () { return &Engine::onCharInput; }, [] (sol::protected_function func) { Engine::onCharInput.add("luaFunction", func);})
            );

    // sol::meta_function::call and other metafunctions are automatically generated: https://sol2.readthedocs.io/en/latest/api/usertype.html
    lua.new_usertype<FunctionSubscribe<void()>>("FunctionSubscribe_V",
            sol::default_constructor,
            "call", &FunctionSubscribe<void()>::call,
            "add", (bool (FunctionSubscribe<void()>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void()>::add
            );

    lua.new_usertype<FunctionSubscribe<void(int,float,float)>>("FunctionSubscribe_V_IFF",
            sol::default_constructor,
            "call", &FunctionSubscribe<void(int,float,float)>::call,
            "add", (bool (FunctionSubscribe<void(int,float,float)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,float,float)>::add
            );

    lua.new_usertype<FunctionSubscribe<void(int,float,float,int)>>("FunctionSubscribe_V_IFFI",
            sol::default_constructor,
            "call", &FunctionSubscribe<void(int,float,float,int)>::call,
            "add", (bool (FunctionSubscribe<void(int,float,float,int)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,float,float,int)>::add
            );

    lua.new_usertype<FunctionSubscribe<void(int,bool,int)>>("FunctionSubscribe_V_IBI",
            sol::default_constructor,
            "call", &FunctionSubscribe<void(int,bool,int)>::call,
            "add", (bool (FunctionSubscribe<void(int,bool,int)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(int,bool,int)>::add
            );

    lua.new_usertype<FunctionSubscribe<void(wchar_t)>>("FunctionSubscribe_V_W",
            sol::default_constructor,
            "call", &FunctionSubscribe<void(wchar_t)>::call,
            "add", (bool (FunctionSubscribe<void(wchar_t)>::*)(const std::string&, sol::protected_function))&FunctionSubscribe<void(wchar_t)>::add
            );

    lua.new_usertype<TextureRender>("TextureRender",  
            sol::default_constructor,
            "createTexture", &TextureRender::createTexture,
            "createFramebufferTexture", &TextureRender::createFramebufferTexture,
            "destroyTexture", &TextureRender::destroyTexture
         );

    lua.new_usertype<FramebufferRender>("FramebufferRender",  
            sol::default_constructor,
            "createFramebuffer", &FramebufferRender::createFramebuffer,
            "destroyFramebuffer", &FramebufferRender::destroyFramebuffer,
            "isCreated", &FramebufferRender::isCreated,
            "colorTexture", sol::property(&FramebufferRender::getColorTexture)
         );

    lua.new_usertype<Scene>("Scene",
	     sol::default_constructor,
         "load", &Scene::load,
         "destroy", &Scene::destroy,
         "draw", &Scene::draw,
         "update", &Scene::update,
         "camera", sol::property(&Scene::getCamera, &Scene::setCamera),
         "setCamera", &Scene::setCamera,
         "getCamera", &Scene::getCamera,
         "mainScene", sol::property(&Scene::isMainScene, &Scene::setMainScene),
         "setMainScene", &Scene::setMainScene,
         "isMainScene", &Scene::isMainScene,
         "backgroundColor", sol::property( &Scene::getBackgroundColor, sol::resolve<void(Vector4)>(&Scene::setBackgroundColor)),
         "setBackgroundColor", sol::overload( sol::resolve<void(float, float, float)>(&Scene::setBackgroundColor), sol::resolve<void(Vector4)>(&Scene::setBackgroundColor) ),
         "getBackgroundColor", &Scene::getBackgroundColor,
         "shadowsPCF", sol::property(&Scene::isShadowsPCF, &Scene::setShadowsPCF),
         "setShadowsPCF", &Scene::setShadowsPCF,
         "isShadowsPCF", &Scene::isShadowsPCF,
         "fogEnabled", sol::property(&Scene::isFogEnabled, &Scene::setFogEnabled),
         "setFogEnabled", &Scene::setFogEnabled,
         "isFogEnabled", &Scene::isFogEnabled,
         "fog", sol::property(&Scene::getFog),
         "getFog", &Scene::getFog,
         "ambientLightColor", sol::property(&Scene::getAmbientLightColor, sol::resolve<void(Vector3)>(&Scene::setAmbientLight)),
         "setAmbientLight", sol::overload( sol::resolve<void(float)>(&Scene::setAmbientLight), sol::resolve<void(Vector3)>(&Scene::setAmbientLight), sol::resolve<void(float, Vector3)>(&Scene::setAmbientLight) ),
         "getAmbientLightColor", &Scene::getAmbientLightColor,
         "ambientLightFactor", sol::property(&Scene::getAmbientLightFactor, sol::resolve<void(float)>(&Scene::setAmbientLight)),
         "getAmbientLightFactor", &Scene::getAmbientLightFactor,
         "sceneAmbientLightEnabled", sol::property(&Scene::isSceneAmbientLightEnabled, &Scene::setSceneAmbientLightEnabled),
         "isSceneAmbientLightEnabled", &Scene::isSceneAmbientLightEnabled,
         "setSceneAmbientLightEnabled", &Scene::setSceneAmbientLightEnabled,
         "renderToTexture", sol::property(&Scene::isRenderToTexture, &Scene::setRenderToTexture),
         "setRenderToTexture", &Scene::setRenderToTexture,
         "isRenderToTexture", &Scene::isRenderToTexture,
         "getFramebuffer", &Scene::getFramebuffer,
         "setFramebufferSize", &Scene::setFramebufferSize,
         "getFramebufferWidth", &Scene::getFramebufferWidth,
         "getFramebufferHeight", &Scene::getFramebufferHeight,
         "updateCameraSize", &Scene::updateCameraSize,
         "findBranchLastIndex", &Scene::findBranchLastIndex,
         "createEntity", &Scene::createEntity,
         "destroyEntity", &Scene::destroyEntity,
         "addEntityChild", &Scene::addEntityChild,
         "moveChildToFirst", &Scene::moveChildToFirst,
         "moveChildUp", &Scene::moveChildUp,
         "moveChildDown", &Scene::moveChildDown,
         "moveChildToLast", &Scene::moveChildToLast
         );

    registerActionClasses(L);
    registerCoreClasses(L);
    registerECSClasses(L);
    registerIOClasses(L);
    registerMathClasses(L);
    registerObjectClasses(L);
    registerUtilClasses(L);
}
