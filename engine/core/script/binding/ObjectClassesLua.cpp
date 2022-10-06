//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "EnumWrapper.h"

#include "Fog.h"
#include "Object.h"
#include "Camera.h"
#include "Polygon.h"
#include "Terrain.h"
#include "Light.h"
#include "Mesh.h"
#include "Fog.h"
#include "Bone.h"
#include "Model.h"
#include "MeshPolygon.h"
#include "Particles.h"
#include "PlaneTerrain.h"
#include "Sprite.h"
#include "Text.h"
#include "Image.h"
#include "Button.h"
#include "TextEdit.h"

using namespace Supernova;

namespace luabridge
{
    template<> struct Stack<FogType> : EnumWrapper<FogType>{};
    template<> struct Stack<CameraType> : EnumWrapper<CameraType>{};
    template<> struct Stack<FrustumPlane> : EnumWrapper<FrustumPlane>{};
    template<> struct Stack<LightType> : EnumWrapper<LightType>{};
}

void LuaBinding::registerObjectClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("FogType")
        .addProperty("LINEAR", FogType::LINEAR)
        .addProperty("EXPONENTIAL", FogType::EXPONENTIAL)
        .addProperty("EXPONENTIALSQUARED", FogType::EXPONENTIALSQUARED)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("CameraType")
        .addProperty("CAMERA_2D", CameraType::CAMERA_2D)
        .addProperty("CAMERA_ORTHO", CameraType::CAMERA_ORTHO)
        .addProperty("CAMERA_PERSPECTIVE", CameraType::CAMERA_PERSPECTIVE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("FrustumPlane")
        .addProperty("FRUSTUM_PLANE_NEAR", FrustumPlane::FRUSTUM_PLANE_NEAR)
        .addProperty("FRUSTUM_PLANE_FAR", FrustumPlane::FRUSTUM_PLANE_FAR)
        .addProperty("FRUSTUM_PLANE_LEFT", FrustumPlane::FRUSTUM_PLANE_LEFT)
        .addProperty("FRUSTUM_PLANE_RIGHT", FrustumPlane::FRUSTUM_PLANE_RIGHT)
        .addProperty("FRUSTUM_PLANE_TOP", FrustumPlane::FRUSTUM_PLANE_TOP)
        .addProperty("FRUSTUM_PLANE_BOTTOM", FrustumPlane::FRUSTUM_PLANE_BOTTOM)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("LightType")
        .addProperty("DIRECTIONAL", LightType::DIRECTIONAL)
        .addProperty("POINT", LightType::POINT)
        .addProperty("SPOT", LightType::SPOT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<Fog>("Fog")
        .addConstructor <void (*) (void)> ()
        .addProperty("type", &Fog::getType, &Fog::setType)
        .addProperty("color", &Fog::getColor, &Fog::setColor)
        .addProperty("density", &Fog::getDensity, &Fog::setDensity)
        .addProperty("linearStart", &Fog::getLinearStart, &Fog::setLinearStart)
        .addProperty("linearEnd", &Fog::getLinearEnd, &Fog::setLinearEnd)
        .addFunction("setLinearStartEnd", &Fog::setLinearStartEnd)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Object>("Object")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("createChild", &Object::createChild)
        .addFunction("addChild", &Object::addChild)
        .addFunction("moveToFirst", &Object::moveToFirst)
        .addFunction("moveUp", &Object::moveUp)
        .addFunction("moveDown", &Object::moveDown)
        .addFunction("moveToLast", &Object::moveToLast)
        .addProperty("name", &Object::getName, &Object::setName)
        .addProperty("position", &Object::getPosition, (void(Object::*)(Vector3))&Object::setPosition)
        .addFunction("setPosition", (void(Object::*)(const float, const float, const float))&Object::setPosition)
        .addFunction("getWorldPosition", &Object::getWorldPosition)
        .addProperty("rotation", &Object::getRotation, (void(Object::*)(Quaternion))&Object::setRotation)
        .addFunction("setRotation", (void(Object::*)(const float, const float, const float))&Object::setRotation)
        .addFunction("getWorldRotation", &Object::getWorldRotation)
        .addProperty("scale", &Object::getScale, (void(Object::*)(Vector3))&Object::setScale)
        .addFunction("setScale", (void(Object::*)(const float))&Object::setScale)
        .addFunction("getWorldScale", &Object::getWorldScale)
        .addFunction("setModelMatrix", &Object::setModelMatrix)
        .addProperty("entity", &Object::getEntity)
        .addFunction("updateTransform", &Object::updateTransform)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Camera, Object>("Camera")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("activate", &Camera::activate)
        .addFunction("setOrtho", &Camera::setOrtho)
        .addFunction("setPerspective", &Camera::setPerspective)
        .addProperty("type",  &Camera::getType, &Camera::setType)
        .addFunction("setType", &Camera::setType)
        .addProperty("view",  &Camera::getView, (void(Camera::*)(Vector3))&Camera::setView)
        .addFunction("setView", (void(Camera::*)(const float, const float, const float))&Camera::setView)
        .addProperty("up",  &Camera::getUp, (void(Camera::*)(Vector3))&Camera::setUp)
        .addFunction("setUp", (void(Camera::*)(const float, const float, const float))&Camera::setUp)
        .addFunction("rotateView", &Camera::rotateView)
        .addFunction("rotatePosition", &Camera::rotatePosition)
        .addFunction("elevateView", &Camera::elevateView)
        .addFunction("elevatePosition", &Camera::elevatePosition)
        .addFunction("moveForward", &Camera::moveForward)
        .addFunction("walkForward", &Camera::walkForward)
        .addFunction("slide", &Camera::slide)
        .addFunction("updateCamera", &Camera::updateCamera)
        .endClass();


    luabridge::getGlobalNamespace(L)
        .deriveClass<Light, Object>("Light")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("type", &Light::getType, &Light::setType)
        .addFunction("setType", &Light::setType)
        .addProperty("direction", &Light::getDirection, (void(Light::*)(Vector3))&Light::setDirection)
        .addFunction("setDirection", (void(Light::*)(const float, const float, const float))&Light::setDirection)
        .addProperty("color", &Light::getColor, (void(Light::*)(Vector3))&Light::setColor)
        .addFunction("setColor", (void(Light::*)(const float, const float, const float))&Light::setColor)
        .addProperty("range", &Light::getRange, &Light::setRange)
        .addFunction("setRange", &Light::setRange)
        .addProperty("intensity", &Light::getIntensity, &Light::setIntensity)
        .addFunction("setIntensity", &Light::setIntensity)
        .addFunction("setConeAngle", &Light::setConeAngle)
        .addProperty("innerConeAngle", &Light::getInnerConeAngle, &Light::setInnerConeAngle)
        .addFunction("setInnerConeAngle", &Light::setInnerConeAngle)
        .addProperty("outerConeAngle", &Light::getOuterConeAngle, &Light::setOuterConeAngle)
        .addFunction("setOuterConeAngle", &Light::setOuterConeAngle)
        .addProperty("shadows", &Light::isShadows, &Light::setShadows)
        .addFunction("setShadows", &Light::setShadows)
        .addFunction("setShadowCameraNearFar", &Light::setShadowCameraNearFar)
        .addProperty("cameraNear", &Light::getCameraNear, &Light::setCameraNear)
        .addFunction("setCameraNear", &Light::setCameraNear)
        .addProperty("cameraFar", &Light::getCameraFar, &Light::setCameraFar)
        .addFunction("setCameraFar", &Light::setCameraFar)
        .addProperty("numCascades", &Light::getNumCascades, &Light::setNumCascades)
        .addFunction("setNumCascades", &Light::setNumCascades)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Mesh, Object>("Mesh")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setTexture", (void(Mesh::*)(std::string))&Mesh::setTexture)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Terrain, Object>("Terrain")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setHeightMap", (void(Terrain::*)(std::string))&Terrain::setHeightMap)
        .addFunction("setBlendMap", (void(Terrain::*)(std::string))&Terrain::setBlendMap)
        .addFunction("setTextureDetailRed", (void(Terrain::*)(std::string))&Terrain::setTextureDetailRed)
        .addFunction("setTextureDetailGreen", (void(Terrain::*)(std::string))&Terrain::setTextureDetailGreen)
        .addFunction("setTextureDetailBlue", (void(Terrain::*)(std::string))&Terrain::setTextureDetailBlue)
        .addFunction("setTexture", (void(Terrain::*)(std::string))&Terrain::setTexture)
        .addProperty("color", &Terrain::getColor, (void(Terrain::*)(Vector4))&Terrain::setColor)
        .addFunction("setColor", (void(Terrain::*)(float, float, float, float))&Terrain::setColor)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Bone, Object>("Bone")
        .addConstructor <void (*) (Scene*, Entity)> ()
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Model, Mesh>("Model")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("loadOBJ", &Model::loadOBJ)
        .addFunction("loadGLTF", &Model::loadGLTF)
        .addFunction("loadModel", &Model::loadModel)
        .addFunction("getAnimation", &Model::getAnimation)
        .addFunction("findAnimation", &Model::findAnimation)
        .addFunction("getBone", +[](Model* self, lua_State* L) -> Bone { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, -1)) return self->getBone(lua_tointeger(L, -1));
            if (lua_isstring(L, -1)) return self->getBone(lua_tostring(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("getMorphWeight", +[](Model* self, lua_State* L) -> float { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, -1)) return self->getMorphWeight(lua_tointeger(L, -1));
            if (lua_isstring(L, -1)) return self->getMorphWeight(lua_tostring(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("setMorphWeight", +[](Model* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 3) throw luaL_error(L, "incorrect argument number");
            if (lua_isstring(L, 2) && lua_isnumber(L, 3)) self->setMorphWeight(lua_tostring(L, 2), lua_tonumber(L, 3));
            else if (lua_isinteger(L, 2) && lua_isnumber(L, 3)) self->setMorphWeight(lua_tointeger(L, 2), lua_tonumber(L, 3));
            else throw luaL_error(L, "incorrect argument type");
            })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<MeshPolygon, Mesh>("MeshPolygon")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addVertex", +[](MeshPolygon* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2 && lua_gettop(L) != 3) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) self->addVertex(lua_tonumber(L, 2), lua_tonumber(L, 3));
            else if (luabridge::Stack<Vector3>::isInstance(L, -1)) self->addVertex(luabridge::Stack<Vector3>::get(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addProperty("width", [] (MeshPolygon* self) -> int { return self->getWidth(); })
        .addProperty("height", [] (MeshPolygon* self) -> int { return self->getHeight(); })
        .addProperty("flipY", &MeshPolygon::isFlipY, &MeshPolygon::setFlipY)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Particles, Object>("Particles")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("maxParticles", &Particles::getMaxParticles, &Particles::setMaxParticles)
        .addFunction("addParticle", +[](Particles* self, lua_State* L) -> void { 
            if (lua_gettop(L) > 6) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<Vector3>::isInstance(L, 2) && luabridge::Stack<Vector4>::isInstance(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5) && luabridge::Stack<Rect>::isInstance(L, 6)) 
                self->addParticle(luabridge::Stack<Vector3>::get(L, 2), luabridge::Stack<Vector4>::get(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), luabridge::Stack<Rect>::get(L, 6));
            else if (luabridge::Stack<Vector3>::isInstance(L, 2) && luabridge::Stack<Vector4>::isInstance(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5))
                self->addParticle(luabridge::Stack<Vector3>::get(L, 2), luabridge::Stack<Vector4>::get(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
            else if (luabridge::Stack<Vector3>::isInstance(L, 2) && luabridge::Stack<Vector4>::isInstance(L, 3))
                self->addParticle(luabridge::Stack<Vector3>::get(L, 2), luabridge::Stack<Vector4>::get(L, 3));
            else if (luabridge::Stack<Vector3>::isInstance(L, 2))
                self->addParticle(luabridge::Stack<Vector3>::get(L, 2));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("addSpriteFrame", +[](Particles* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 6) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) 
                self->addSpriteFrame(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
            else if (lua_isstring(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6))
                self->addSpriteFrame(lua_tostring(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
            else if (lua_isinteger(L, 2) && lua_isstring(L, 3) && luabridge::Stack<Rect>::isInstance(L, 4))
                self->addSpriteFrame(lua_tointeger(L, 2), lua_tostring(L, 3), luabridge::Stack<Rect>::get(L, 4));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("removeSpriteFrame", +[](Particles* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, -1)) self->removeSpriteFrame(lua_tointeger(L, -1));
            else if (lua_isstring(L, -1)) self->removeSpriteFrame(lua_tostring(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("setTexture", (void(Particles::*)(std::string))&Particles::setTexture)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<PlaneTerrain, Mesh>("PlaneTerrain")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("create", &PlaneTerrain::create)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Sprite, Mesh>("Sprite")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &Sprite::setSize)
        .addProperty("width", &Sprite::getWidth, &Sprite::setWidth)
        .addProperty("height", &Sprite::getHeight, &Sprite::setHeight)
        .addProperty("flipY", &Sprite::isFlipY, &Sprite::setFlipY)
        .addFunction("setBillboard", &Sprite::setBillboard)
        .addProperty("flipY", &Sprite::isFlipY, &Sprite::setFlipY)
        .addProperty("textureRect", &Sprite::getTextureRect, (void(Sprite::*)(Rect))&Sprite::setTextureRect)
        .addFunction("setTextureRect",(void(Sprite::*)(float, float, float, float)) &Sprite::setTextureRect)
        .addFunction("addFrame", +[](Sprite* self, lua_State* L) -> void { 
            if (lua_gettop(L) > 6) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, 2) && lua_isstring(L, 3) && luabridge::Stack<Rect>::isInstance(L, 4))
                self->addFrame(lua_tointeger(L, 2), lua_tostring(L, 3), luabridge::Stack<Rect>::get(L, 4));
            else if (lua_isstring(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6))
                self->addFrame(lua_tostring(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
            else if (lua_isinteger(L, 2) && lua_isstring(L, 3) && luabridge::Stack<Rect>::isInstance(L, 4))
                self->addFrame(lua_tointeger(L, 2), lua_tostring(L, 3), luabridge::Stack<Rect>::get(L, 4));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("removeFrame", +[](Sprite* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, -1)) self->removeFrame(lua_tointeger(L, -1));
            else if (lua_isstring(L, -1)) self->removeFrame(lua_tostring(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("setFrame", +[](Sprite* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isinteger(L, -1)) self->setFrame(lua_tointeger(L, -1));
            else if (lua_isstring(L, -1)) self->setFrame(lua_tostring(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("startAnimation", +[](Sprite* self, lua_State* L) -> void { 
            if (lua_gettop(L) > 5) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<std::vector<int>>::isInstance(L, 2) && luabridge::Stack<std::vector<int>>::isInstance(L, 3) && lua_isboolean(L, 4))
                self->startAnimation(luabridge::Stack<std::vector<int>>::get(L, 2), luabridge::Stack<std::vector<int>>::get(L, 3), lua_toboolean(L, 4));
            else if (lua_isinteger(L, 2) && lua_isinteger(L, 3) && lua_isinteger(L, 4) && lua_isboolean(L, 5)) 
                self->startAnimation(lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4), lua_toboolean(L, 5));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("pauseAnimation", &Sprite::pauseAnimation)
        .addFunction("stopAnimation", &Sprite::stopAnimation)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Polygon, Object>("Polygon")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addVertex", +[](Polygon* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2 && lua_gettop(L) != 3) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) self->addVertex(lua_tonumber(L, 2), lua_tonumber(L, 3));
            else if (luabridge::Stack<Vector3>::isInstance(L, -1)) self->addVertex(luabridge::Stack<Vector3>::get(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addProperty("color", &Polygon::getColor, (void(Polygon::*)(Vector4))&Polygon::setColor)
        .addFunction("setColor", (void(Polygon::*)(float, float, float, float))&Polygon::setColor)
        .addFunction("setTexture", (void(Polygon::*)(std::string))&Polygon::setTexture)
        .addProperty("width", [] (Polygon* self) -> int { return self->getWidth(); })
        .addProperty("height", [] (Polygon* self) -> int { return self->getHeight(); })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Text, Object>("Text")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &Text::setSize)
        .addProperty("width", &Text::getWidth, &Text::setWidth)
        .addProperty("height", &Text::getHeight, &Text::setHeight)
        .addProperty("maxTextSize", &Text::getMaxTextSize, &Text::setMaxTextSize)
        .addProperty("text", &Text::getText, &Text::setText)
        .addProperty("font", &Text::getFont, &Text::setFont)
        .addProperty("fontSize", &Text::getFontSize, &Text::setFontSize)
        .addProperty("multiline", &Text::getMultiline, &Text::setMultiline)
        .addProperty("color", &Text::getColor, (void(Text::*)(Vector4))&Text::setColor)
        .addFunction("setColor", (void(Text::*)(float, float, float, float))&Text::setColor)
        .addFunction("getAscent", &Text::getAscent)
        .addFunction("getDescent", &Text::getDescent)
        .addFunction("getLineGap", &Text::getLineGap)
        .addFunction("getLineHeight", &Text::getLineHeight)
        .addFunction("getNumChars", &Text::getNumChars)
        .addFunction("getCharPosition", &Text::getCharPosition)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Image, Object>("Image")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &Image::setSize)
        .addProperty("width", &Image::getWidth, &Image::setWidth)
        .addProperty("height", &Image::getHeight, &Image::setHeight)
        .addFunction("setMargin", &Image::setMargin)
        .addProperty("marginBottom", &Image::getMarginBottom, &Image::setMarginBottom)
        .addProperty("marginLeft", &Image::getMarginLeft, &Image::setMarginLeft)
        .addProperty("marginRight", &Image::getMarginRight, &Image::setMarginRight)
        .addProperty("marginTop", &Image::getMarginTop, &Image::setMarginTop)
        .addFunction("setTexture", (void(Image::*)(std::string))&Image::setTexture)
        .addProperty("color", &Image::getColor, (void(Image::*)(Vector4))&Image::setColor)
        .addFunction("setColor", (void(Image::*)(const float, const float, const float, const float))&Image::setColor)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Button, Image>("Button")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("getLabelObject", &Button::getLabelObject)
        .addProperty("label", &Button::getLabel, &Button::setLabel)
        .addProperty("labelColor", &Button::getLabelColor, (void(Button::*)(Vector4))&Button::setLabelColor)
        .addFunction("setLabelColor", (void(Button::*)(const float, const float, const float, const float))&Button::setLabelColor)
        .addProperty("labelFont", &Button::getLabelFont, &Button::setLabelFont)
        .addProperty("fontSize", &Button::getFontSize, &Button::setFontSize)
        .addFunction("setTextureNormal", (void(Button::*)(std::string))&Button::setTextureNormal)
        .addFunction("setTexturePressed", (void(Button::*)(std::string))&Button::setTexturePressed)
        .addFunction("setTextureDisabled", (void(Button::*)(std::string))&Button::setTextureDisabled)
        .addProperty("disabled", &Button::getDisabled, &Button::setDisabled)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<TextEdit, Image>("TextEdit")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("getTextObject", &TextEdit::getTextObject)
        .addProperty("disabled", &TextEdit::getDisabled, &TextEdit::setDisabled)
        .addProperty("text", &TextEdit::getText, &TextEdit::setText)
        .addProperty("textColor", &TextEdit::getTextColor, (void(TextEdit::*)(Vector4))&TextEdit::setTextColor)
        .addFunction("setTextColor", (void(TextEdit::*)(const float, const float, const float, const float))&TextEdit::setTextColor)
        .addProperty("textFont", &TextEdit::getTextFont, &TextEdit::setTextFont)
        .addProperty("fontSize", &TextEdit::getFontSize, &TextEdit::setFontSize)
        .addProperty("maxTextSize", &TextEdit::getMaxTextSize, &TextEdit::setMaxTextSize)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}