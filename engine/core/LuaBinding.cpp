
#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaIntf.h"

#include "Supernova.h"
#include "Object.h"
#include "ConcreteObject.h"
#include "platform/Log.h"
#include "Scene.h"
#include "Shape.h"
#include "Cube.h"
#include "Plane.h"
#include "Model.h"
#include "math/Ray.h"
#include "math/Quaternion.h"
#include "Input.h"
#include "Mesh.h"
#include "Mesh2D.h"
#include "Image.h"
#include "GUIObject.h"
#include "ImageTemplate.h"
#include "Light.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Sound.h"
#include "SkyBox.h"
#include <map>
#include <unistd.h>

/*
namespace LuaIntf
{
	LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)
    LUA_USING_LIST_TYPE(std::vector)
    LUA_USING_MAP_TYPE(std::map)
}
*/

LuaBinding::LuaBinding() {
    // TODO Auto-generated constructor stub

}

LuaBinding::~LuaBinding() {
    // TODO Auto-generated destructor stub
}

int LuaBinding::setLuaPath(const char* path)
{
    lua_State *L = Supernova::getLuaState();

    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    std::string cur_path = lua_tostring( L, -1 );
    cur_path.append( ";" );
    cur_path.append( path );
    lua_pop( L, 1 );
    lua_pushstring( L, cur_path.c_str() );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );
    return 0;
}


void LuaBinding::bind(){


    lua_State *L = Supernova::getLuaState();
    luaL_openlibs(L);


    LuaIntf::LuaBinding(L).beginClass<Supernova>("Supernova")
    .addConstructor(LUA_ARGS())
    .addStaticFunction("setScene", &Supernova::setScene)
    .addStaticFunction("getCanvasWidth", &Supernova::getCanvasWidth)
    .addStaticFunction("getCanvasHeight", &Supernova::getCanvasHeight)
    .addStaticFunction("setCanvasSize", &Supernova::setCanvasSize)
    .addStaticFunction("setMouseAsTouch", &Supernova::setMouseAsTouch)
    .addStaticFunction("setScalingMode", &Supernova::setScalingMode)
    .addStaticFunction("onFrame", static_cast<int(*)(lua_State*)>(&Supernova::onFrame))
    .addStaticFunction("onTouchPress", static_cast<int(*)(lua_State*)>(&Supernova::onTouchPress))
    .addStaticFunction("onTouchUp", static_cast<int(*)(lua_State*)>(&Supernova::onTouchUp))
    .addStaticFunction("onTouchDrag", static_cast<int(*)(lua_State*)>(&Supernova::onTouchDrag))
    .addStaticFunction("onMousePress", static_cast<int(*)(lua_State*)>(&Supernova::onMousePress))
    .addStaticFunction("onMouseUp", static_cast<int(*)(lua_State*)>(&Supernova::onMouseUp))
    .addStaticFunction("onMouseDrag", static_cast<int(*)(lua_State*)>(&Supernova::onMouseDrag))
    .addStaticFunction("onMouseMove", static_cast<int(*)(lua_State*)>(&Supernova::onMouseMove))
    .addStaticFunction("onKeyPress", static_cast<int(*)(lua_State*)>(&Supernova::onKeyPress))
    .addStaticFunction("onKeyUp", static_cast<int(*)(lua_State*)>(&Supernova::onKeyUp))
    .addConstant("SCALING_FITWIDTH", S_SCALING_FITWIDTH)
    .addConstant("SCALING_FITHEIGHT", S_SCALING_FITHEIGHT)
    .addConstant("SCALING_LETTERBOX", S_SCALING_LETTERBOX)
    .addConstant("SCALING_CROP", S_SCALING_CROP)
    .addConstant("SCALING_STRETCH", S_SCALING_STRETCH)
    .endClass();

    LuaIntf::LuaBinding(L).beginClass<Object>("Object")
    .addFunction("addObject", &Object::addObject)
    .addFunction("setPosition", (void (Object::*)(const float, const float, const float))&Object::setPosition)
    .addFunction("getPosition", &Object::getPosition)
    .addProperty("position", &Object::getPosition, (void (Object::*)(Vector3))&Object::setPosition)
    .addFunction("setRotation", (void (Object::*)(const float, const float, const float))&Object::setRotation)
    .addFunction("getRotation", &Object::getRotation)
    .addProperty("rotation", &Object::getRotation, (void (Object::*)(Quaternion))&Object::setRotation)
    .addFunction("setScale", (void (Object::*)(const float))&Object::setScale)
    .addFunction("getScale", &Object::getScale)
    .addProperty("scale", &Object::getScale, (void (Object::*)(Vector3))&Object::setScale)
    .addFunction("setCenter", (void (Object::*)(const float, const float, const float))&Object::setCenter)
    .addFunction("getCenter", &Object::getCenter)
    .addFunction("moveToFront", &Object::moveToFront)
    .addFunction("moveToBack", &Object::moveToBack)
    .addFunction("moveUp", &Object::moveUp)
    .addFunction("moveDown", &Object::moveDown)
    .addProperty("center", &Object::getCenter, (void (Object::*)(Vector3))&Object::setCenter)
    .addFunction("destroy", &Object::destroy)
    .endClass()

    .beginExtendClass<Scene, Object>("Scene")
    .addConstructor(LUA_ARGS())
    .addFunction("setCamera", &Scene::setCamera)
    .addFunction("addObject", &Scene::addObject)
    .addFunction("setAmbientLight", (void (Scene::*)(const float))&Scene::setAmbientLight)
    .addProperty("ambientLight", &Scene::getAmbientLight, (void (Scene::*)(Vector3))&Scene::setAmbientLight)
    .endClass()

    .beginExtendClass<Camera, Object>("Camera")
    .addConstructor(LUA_ARGS())
    .addFunction("setView", (void (Camera::*)(const float, const float, const float))&Camera::setView)
    .addFunction("getView", &Camera::getView)
    .addProperty("view", &Camera::getView, (void (Camera::*)(Vector3))&Camera::setView)
    .addFunction("setUp", (void (Camera::*)(const float, const float, const float))&Camera::setUp)
    .addFunction("getUp", &Camera::getUp)
    .addProperty("up", &Camera::getUp, (void (Camera::*)(Vector3))&Camera::setUp)
    .addFunction("setProjection", &Camera::setProjection)
    .addFunction("setPerspective", &Camera::setPerspective)
    .addFunction("setOrtho", &Camera::setOrtho)
    .addFunction("pointsToRay", &Camera::pointsToRay)
    .addFunction("rotateView", &Camera::rotateView)
    .addFunction("rotatePosition", &Camera::rotatePosition)
    .addFunction("elevateView", &Camera::elevateView)
    .addFunction("elevatePosition", &Camera::elevatePosition)
    .addFunction("moveForward", &Camera::moveForward)
    .addFunction("walkForward", &Camera::walkForward)
    .addFunction("slide", &Camera::slide)
    .addConstant("ORTHO", S_ORTHO)
    .addConstant("PERSPECTIVE", S_PERSPECTIVE)
    .endClass()

    .beginExtendClass<Light, Object>("Light")
    .addConstructor(LUA_ARGS())
    .addFunction("getColor", &Light::getColor)
    .addFunction("getTarget", &Light::getTarget)
    .addFunction("getDirection", &Light::getDirection)
    .addFunction("getPower", &Light::getPower)
    .addFunction("getSpotAngle", &Light::getSpotAngle)
    .addFunction("setPower", &Light::setPower)
    .endClass()

    .beginExtendClass<PointLight, Light>("PointLight")
    .addConstructor(LUA_ARGS())
    .endClass()

    .beginExtendClass<SpotLight, Light>("SpotLight")
    .addConstructor(LUA_ARGS())
    .addFunction("setTarget", (void (SpotLight::*)(const float, const float, const float))&SpotLight::setTarget)
    .addProperty("target", &SpotLight::getTarget, (void (SpotLight::*)(Vector3))&SpotLight::setTarget)
    .addFunction("setSpotAngle", &SpotLight::setSpotAngle)
    .endClass()

    .beginExtendClass<DirectionalLight, Light>("DirectionalLight")
    .addConstructor(LUA_ARGS())
    .addFunction("setDirection", (void (DirectionalLight::*)(const float, const float, const float))&DirectionalLight::setDirection)
    .addProperty("target", &DirectionalLight::getDirection, (void (DirectionalLight::*)(Vector3))&DirectionalLight::setDirection)
    .endClass()

    .beginExtendClass<ConcreteObject, Object>("ConcreteObject")
    .addFunction("setTexture", (void (ConcreteObject::*)(std::string))&ConcreteObject::setTexture)
    .addFunction("setColor", &ConcreteObject::setColor)
    .endClass()

    .beginExtendClass<Mesh, ConcreteObject>("Mesh")
    .addConstructor(LUA_ARGS())
    .endClass()

    .beginExtendClass<SkyBox, Mesh>("SkyBox")
    .addConstructor(LUA_ARGS())
    .addFunction("setTextureFront", &SkyBox::setTextureFront)
    .addFunction("setTextureBack", &SkyBox::setTextureBack)
    .addFunction("setTextureLeft", &SkyBox::setTextureLeft)
    .addFunction("setTextureRight", &SkyBox::setTextureRight)
    .addFunction("setTextureUp", &SkyBox::setTextureUp)
    .addFunction("setTextureDown", &SkyBox::setTextureDown)
    .endClass()

    .beginExtendClass<Mesh2D, Mesh>("Mesh2D")
    .addFunction("setSize", &Mesh2D::setSize)
    .addFunction("setBillboard", &Mesh2D::setBillboard)
    .endClass()

    .beginExtendClass<ImageTemplate, Mesh2D>("ImageTemplate")
    .addConstructor(LUA_ARGS())
    .endClass()
    
    .beginExtendClass<Image, Mesh2D>("Image")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<const char *>))
    .endClass()

    .beginExtendClass<Shape, Mesh>("Shape")
    .addConstructor(LUA_ARGS())
    .addFunction("addVertex", (void (Shape::*)(float, float))&Shape::addVertex)
    //.addFunction("addVertex", LUA_FN(void, Shape::addVertex, float))
    .endClass()

    .beginExtendClass<Cube, Mesh>("Cube")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<float>, LuaIntf::_opt<float>, LuaIntf::_opt<float>))
    .endClass()

    .beginExtendClass<Plane, Mesh>("Plane")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<float>, LuaIntf::_opt<float>))
    .endClass()

    .beginExtendClass<Model, Mesh>("Model")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<const char *>))
    .endClass();

    LuaIntf::LuaBinding(L).beginClass<Vector3>("Vector3")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<float>, LuaIntf::_opt<float>, LuaIntf::_opt<float>))
    .addVariable("x", &Vector3::x)
    .addVariable("y", &Vector3::y)
    .addVariable("z", &Vector3::z)
    .endClass();

    LuaIntf::LuaBinding(L).beginClass<Quaternion>("Quaternion")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<float>, LuaIntf::_opt<float>, LuaIntf::_opt<float>, LuaIntf::_opt<float>))
    .addVariable("w", &Quaternion::w)
    .addVariable("x", &Quaternion::x)
    .addVariable("y", &Quaternion::y)
    .addVariable("z", &Quaternion::z)
    .endClass();

    LuaIntf::LuaBinding(L).beginClass<Ray>("Ray")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<Vector3>, LuaIntf::_opt<Vector3>))
    .addFunction("intersectionPoint", &Ray::intersectionPoint)
    .endClass();

    LuaIntf::LuaBinding(L).beginClass<Sound>("Sound")
    .addConstructor(LUA_ARGS(LuaIntf::_opt<const char *>))
    .addFunction("load", &Sound::load)
    .addFunction("play", &Sound::play)
    .addFunction("stop", &Sound::stop)
    .endClass();

    LuaIntf::LuaBinding(L).beginModule("Input")
    .addConstant("KEY_SPACE", S_KEY_SPACE)
    .addConstant("KEY_APOSTROPHE", S_KEY_APOSTROPHE)
    .addConstant("KEY_COMMA", S_KEY_COMMA)
    .addConstant("KEY_MINUS", S_KEY_MINUS)
    .addConstant("KEY_PERIOD", S_KEY_PERIOD)
    .addConstant("KEY_SLASH", S_KEY_SLASH)
    .addConstant("KEY_0", S_KEY_0)
    .addConstant("KEY_1", S_KEY_1)
    .addConstant("KEY_2", S_KEY_2)
    .addConstant("KEY_3", S_KEY_3)
    .addConstant("KEY_4", S_KEY_4)
    .addConstant("KEY_5", S_KEY_5)
    .addConstant("KEY_6", S_KEY_6)
    .addConstant("KEY_7", S_KEY_7)
    .addConstant("KEY_8", S_KEY_8)
    .addConstant("KEY_9", S_KEY_9)
    .addConstant("KEY_SEMICOLON", S_KEY_SEMICOLON)
    .addConstant("KEY_EQUAL", S_KEY_EQUAL)
    .addConstant("KEY_A", S_KEY_A)
    .addConstant("KEY_B", S_KEY_B)
    .addConstant("KEY_C", S_KEY_C)
    .addConstant("KEY_D", S_KEY_D)
    .addConstant("KEY_E", S_KEY_E)
    .addConstant("KEY_F", S_KEY_F)
    .addConstant("KEY_G", S_KEY_G)
    .addConstant("KEY_H", S_KEY_H)
    .addConstant("KEY_I", S_KEY_I)
    .addConstant("KEY_J", S_KEY_J)
    .addConstant("KEY_K", S_KEY_K)
    .addConstant("KEY_L", S_KEY_L)
    .addConstant("KEY_M", S_KEY_M)
    .addConstant("KEY_N", S_KEY_N)
    .addConstant("KEY_O", S_KEY_O)
    .addConstant("KEY_P", S_KEY_P)
    .addConstant("KEY_Q", S_KEY_Q)
    .addConstant("KEY_R", S_KEY_R)
    .addConstant("KEY_S", S_KEY_S)
    .addConstant("KEY_T", S_KEY_T)
    .addConstant("KEY_U", S_KEY_U)
    .addConstant("KEY_V", S_KEY_V)
    .addConstant("KEY_W", S_KEY_W)
    .addConstant("KEY_X", S_KEY_X)
    .addConstant("KEY_Y", S_KEY_Y)
    .addConstant("KEY_Z", S_KEY_Z)
    .addConstant("KEY_LEFT_BRACKET", S_KEY_LEFT_BRACKET)
    .addConstant("KEY_BACKSLASH", S_KEY_BACKSLASH)
    .addConstant("KEY_RIGHT_BRACKET", S_KEY_RIGHT_BRACKET)
    .addConstant("KEY_GRAVE_ACCENT", S_KEY_GRAVE_ACCENT)
    .addConstant("KEY_ESCAPE", S_KEY_ESCAPE)
    .addConstant("KEY_ENTER", S_KEY_ENTER)
    .addConstant("KEY_TAB", S_KEY_TAB)
    .addConstant("KEY_BACKSPACE", S_KEY_BACKSPACE)
    .addConstant("KEY_INSERT", S_KEY_INSERT)
    .addConstant("KEY_DELETE", S_KEY_DELETE)
    .addConstant("KEY_RIGHT", S_KEY_RIGHT)
    .addConstant("KEY_LEFT", S_KEY_LEFT)
    .addConstant("KEY_DOWN", S_KEY_DOWN)
    .addConstant("KEY_UP", S_KEY_UP)
    .addConstant("KEY_PAGE_UP", S_KEY_PAGE_UP)
    .addConstant("KEY_PAGE_DOWN", S_KEY_PAGE_DOWN)
    .addConstant("KEY_HOME", S_KEY_HOME)
    .addConstant("KEY_END", S_KEY_END)
    .addConstant("KEY_CAPS_LOCK", S_KEY_CAPS_LOCK)
    .addConstant("KEY_SCROLL_LOCK", S_KEY_SCROLL_LOCK)
    .addConstant("KEY_NUM_LOCK", S_KEY_NUM_LOCK)
    .addConstant("KEY_PRINT_SCREEN", S_KEY_PRINT_SCREEN)
    .addConstant("KEY_PAUSE", S_KEY_PAUSE)
    .addConstant("KEY_F1", S_KEY_F1)
    .addConstant("KEY_F2", S_KEY_F2)
    .addConstant("KEY_F3", S_KEY_F3)
    .addConstant("KEY_F4", S_KEY_F4)
    .addConstant("KEY_F5", S_KEY_F5)
    .addConstant("KEY_F6", S_KEY_F6)
    .addConstant("KEY_F7", S_KEY_F7)
    .addConstant("KEY_F8", S_KEY_F8)
    .addConstant("KEY_F9", S_KEY_F9)
    .addConstant("KEY_F10", S_KEY_F10)
    .addConstant("KEY_F11", S_KEY_F11)
    .addConstant("KEY_F12", S_KEY_F12)
    .addConstant("KEY_KP_0", S_KEY_KP_0)
    .addConstant("KEY_KP_1", S_KEY_KP_1)
    .addConstant("KEY_KP_2", S_KEY_KP_2)
    .addConstant("KEY_KP_3", S_KEY_KP_3)
    .addConstant("KEY_KP_4", S_KEY_KP_4)
    .addConstant("KEY_KP_5", S_KEY_KP_5)
    .addConstant("KEY_KP_6", S_KEY_KP_6)
    .addConstant("KEY_KP_7", S_KEY_KP_7)
    .addConstant("KEY_KP_8", S_KEY_KP_8)
    .addConstant("KEY_KP_9", S_KEY_KP_9)
    .addConstant("KEY_KP_DECIMAL", S_KEY_KP_DECIMAL)
    .addConstant("KEY_KP_DIVIDE", S_KEY_KP_DIVIDE)
    .addConstant("KEY_KP_MULTIPLY", S_KEY_KP_MULTIPLY)
    .addConstant("KEY_KP_SUBTRACT", S_KEY_KP_SUBTRACT)
    .addConstant("KEY_KP_ADD", S_KEY_KP_ADD)
    .addConstant("KEY_KP_ENTER", S_KEY_KP_ENTER)
    .addConstant("KEY_KP_EQUAL", S_KEY_KP_EQUAL)
    .addConstant("KEY_LEFT_SHIFT", S_KEY_LEFT_SHIFT)
    .addConstant("KEY_LEFT_CONTROL", S_KEY_LEFT_CONTROL)
    .addConstant("KEY_LEFT_ALT", S_KEY_LEFT_ALT)
    .addConstant("KEY_LEFT_SUPER", S_KEY_LEFT_SUPER)
    .addConstant("KEY_RIGHT_SHIFT", S_KEY_RIGHT_SHIFT)
    .addConstant("KEY_RIGHT_CONTROL", S_KEY_RIGHT_CONTROL)
    .addConstant("KEY_RIGHT_ALT", S_KEY_RIGHT_ALT)
    .addConstant("KEY_RIGHT_SUPER", S_KEY_RIGHT_SUPER)
    .addConstant("KEY_MENU", S_KEY_MENU)

    .addConstant("MOUSE_BUTTON_1", S_MOUSE_BUTTON_1)
    .addConstant("MOUSE_BUTTON_2", S_MOUSE_BUTTON_2)
    .addConstant("MOUSE_BUTTON_3", S_MOUSE_BUTTON_3)
    .addConstant("MOUSE_BUTTON_4", S_MOUSE_BUTTON_4)
    .addConstant("MOUSE_BUTTON_5", S_MOUSE_BUTTON_5)
    .addConstant("MOUSE_BUTTON_6", S_MOUSE_BUTTON_6)
    .addConstant("MOUSE_BUTTON_7", S_MOUSE_BUTTON_7)
    .addConstant("MOUSE_BUTTON_8", S_MOUSE_BUTTON_8)
    .addConstant("MOUSE_BUTTON_LAST", S_MOUSE_BUTTON_LAST)
    .addConstant("MOUSE_BUTTON_LEFT", S_MOUSE_BUTTON_LEFT)
    .addConstant("MOUSE_BUTTON_RIGHT", S_MOUSE_BUTTON_RIGHT)
    .addConstant("MOUSE_BUTTON_MIDDLE", S_MOUSE_BUTTON_MIDDLE)
    .endModule();


    setLuaPath("lua/?.lua");

    //int luaL_dofile (lua_State *L, const char *filename);
    if (luaL_loadfile(L, "lua/main.lua") == 0){
        if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0){
            Log::Error(LOG_TAG, "Lua Error: %s\n", lua_tostring(L,-1));
            lua_close(L);
        }
    }else{
        Log::Error(LOG_TAG, "Lua Error: %s\n", lua_tostring(L,-1));
        lua_close(L);
    }
}
