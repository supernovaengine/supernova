//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "LuaBridgeAddon.h"

#include "Fog.h"
#include "SkyBox.h"
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
#include "Container.h"
#include "Audio.h"

using namespace Supernova;


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
        .beginClass<SkyBox>("SkyBox")
        .addConstructor <void (Scene*)> ()
        .addFunction("setTextures", &SkyBox::setTextures)
        .addFunction("setTextureFront", &SkyBox::setTextureFront)
        .addFunction("setTextureBack", &SkyBox::setTextureBack)
        .addFunction("setTextureLeft", &SkyBox::setTextureLeft)
        .addFunction("setTextureRight", &SkyBox::setTextureRight)
        .addFunction("setTextureUp", &SkyBox::setTextureUp)
        .addFunction("setTextureDown", &SkyBox::setTextureDown)
        .addProperty("color", &SkyBox::getColor, (void(SkyBox::*)(Vector4))&SkyBox::setColor)
        .addFunction("setColor", (void(SkyBox::*)(const float, const float, const float))&SkyBox::setColor)
        .addProperty("alpha", &SkyBox::getAlpha, &SkyBox::setAlpha)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Object>("Object")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("createChild", &Object::createChild)
        .addFunction("addChild", 
            luabridge::overload<Object*>(&Object::addChild),
            luabridge::overload<Entity>(&Object::addChild))
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
        .addFunction("setBillboard", &Sprite::setBillboard)
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
        .addFunction("getWorldView", &Camera::getWorldView)
        .addFunction("getWorldUp", &Camera::getWorldUp)
        .addFunction("getWorldRight", &Camera::getWorldRight)
        .addFunction("rotateView", &Camera::rotateView)
        .addFunction("rotatePosition", &Camera::rotatePosition)
        .addFunction("elevateView", &Camera::elevateView)
        .addFunction("elevatePosition", &Camera::elevatePosition)
        .addFunction("moveForward", &Camera::moveForward)
        .addFunction("walkForward", &Camera::walkForward)
        .addFunction("slide", &Camera::slide)
        .addProperty("renderToTexture", &Camera::isRenderToTexture, &Camera::setRenderToTexture)
        .addFunction("getFramebuffer", &Camera::getFramebuffer)
        .addFunction("setFramebufferSize", &Camera::setFramebufferSize)
        .addFunction("setFramebufferFilter", &Camera::setFramebufferFilter)
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
        .addFunction("setTexture", 
            luabridge::overload<std::string>(&Mesh::setTexture),
            luabridge::overload<Framebuffer*>(&Mesh::setTexture))
        .addProperty("color", &Mesh::getColor, (void(Mesh::*)(Vector4))&Mesh::setColor)
        .addFunction("setColor", (void(Mesh::*)(const float, const float, const float, const float))&Mesh::setColor)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Terrain, Object>("Terrain")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setHeightMap", (void(Terrain::*)(std::string))&Terrain::setHeightMap)
        .addFunction("setBlendMap", (void(Terrain::*)(std::string))&Terrain::setBlendMap)
        .addFunction("setTextureDetailRed", (void(Terrain::*)(std::string))&Terrain::setTextureDetailRed)
        .addFunction("setTextureDetailGreen", (void(Terrain::*)(std::string))&Terrain::setTextureDetailGreen)
        .addFunction("setTextureDetailBlue", (void(Terrain::*)(std::string))&Terrain::setTextureDetailBlue)
        .addFunction("setTexture", 
            luabridge::overload<std::string>(&Terrain::setTexture),
            luabridge::overload<Framebuffer*>(&Terrain::setTexture))
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
        .addFunction("getBone", 
            luabridge::overload<int>(&Model::getBone),
            luabridge::overload<std::string>(&Model::getBone))
        .addFunction("getMorphWeight", 
            luabridge::overload<int>(&Model::getMorphWeight),
            luabridge::overload<std::string>(&Model::getMorphWeight))
        .addFunction("setMorphWeight", 
            luabridge::overload<int, float>(&Model::setMorphWeight),
            luabridge::overload<std::string, float>(&Model::setMorphWeight))
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<MeshPolygon, Mesh>("MeshPolygon")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addVertex", 
            luabridge::overload<Vector3>(&MeshPolygon::addVertex),
            luabridge::overload<float, float>(&MeshPolygon::addVertex))
        .addProperty("width", [] (MeshPolygon* self) -> int { return self->getWidth(); })
        .addProperty("height", [] (MeshPolygon* self) -> int { return self->getHeight(); })
        .addProperty("flipY", &MeshPolygon::isFlipY, &MeshPolygon::setFlipY)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Particles, Object>("Particles")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("maxParticles", &Particles::getMaxParticles, &Particles::setMaxParticles)
        .addFunction("addParticle", 
            luabridge::overload<Vector3>(&Particles::addParticle),
            luabridge::overload<Vector3, Vector4>(&Particles::addParticle),
            luabridge::overload<Vector3, Vector4, float, float>(&Particles::addParticle),
            luabridge::overload<Vector3, Vector4, float, float, Rect>(&Particles::addParticle),
            luabridge::overload<float, float, float>(&Particles::addParticle))
        .addFunction("addSpriteFrame", 
            luabridge::overload<int, std::string, Rect>(&Particles::addSpriteFrame),
            luabridge::overload<std::string, float, float, float, float>(&Particles::addSpriteFrame),
            luabridge::overload<float, float, float, float>(&Particles::addSpriteFrame),
            luabridge::overload<Rect>(&Particles::addSpriteFrame))
        .addFunction("removeSpriteFrame", 
            luabridge::overload<int>(&Particles::removeSpriteFrame),
            luabridge::overload<std::string>(&Particles::removeSpriteFrame))
        .addFunction("setTexture", 
            luabridge::overload<std::string>(&Particles::setTexture),
            luabridge::overload<Framebuffer*>(&Particles::setTexture))
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
        .addProperty("textureRect", &Sprite::getTextureRect, (void(Sprite::*)(Rect))&Sprite::setTextureRect)
        .addFunction("setTextureRect",(void(Sprite::*)(float, float, float, float)) &Sprite::setTextureRect)
        .addProperty("pivot", &Sprite::getPivot, &Sprite::setPivot)
        .addFunction("addFrame", 
            luabridge::overload<int, std::string, Rect>(&Sprite::addFrame),
            luabridge::overload<std::string, float, float, float, float>(&Sprite::addFrame),
            luabridge::overload<float, float, float, float>(&Sprite::addFrame),
            luabridge::overload<Rect>(&Sprite::addFrame))
        .addFunction("removeFrame", 
            luabridge::overload<int>(&Sprite::removeFrame),
            luabridge::overload<std::string>(&Sprite::removeFrame))
        .addFunction("setFrame", 
            luabridge::overload<int>(&Sprite::setFrame),
            luabridge::overload<std::string>(&Sprite::setFrame))
        .addFunction("startAnimation", 
            luabridge::overload<std::vector<int>, std::vector<int>, bool>(&Sprite::startAnimation),
            luabridge::overload<int, int, int, bool>(&Sprite::startAnimation))
        .addFunction("pauseAnimation", &Sprite::pauseAnimation)
        .addFunction("stopAnimation", &Sprite::stopAnimation)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<UILayout, Object>("UILayout")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &UILayout::setSize)
        .addProperty("width", &UILayout::getWidth, &Image::setWidth)
        .addProperty("height", &UILayout::getHeight, &UILayout::setHeight)
        .addFunction("setAnchors", &UILayout::setAnchors)
        .addProperty("anchorLeft", &UILayout::getAnchorLeft, &UILayout::setAnchorLeft)
        .addProperty("anchorTop", &UILayout::getAnchorTop, &UILayout::setAnchorTop)
        .addProperty("anchorRight", &UILayout::getAnchorRight, &UILayout::setAnchorRight)
        .addProperty("anchorBottom", &UILayout::getAnchorBottom, &UILayout::setAnchorBottom)
        .addFunction("setMargins", &UILayout::setMargins)
        .addProperty("marginLeft", &UILayout::getMarginLeft, &UILayout::setMarginLeft)
        .addProperty("marginTop", &UILayout::getMarginTop, &UILayout::setMarginTop)
        .addProperty("marginRight", &UILayout::getMarginRight, &UILayout::setMarginRight)
        .addProperty("marginBottom", &UILayout::getMarginBottom, &UILayout::setMarginBottom)
        .addProperty("anchorPreset", &UILayout::getAnchorPreset, &UILayout::setAnchorPreset)
        .addProperty("usingAnchors", &UILayout::isUsingAnchors, &UILayout::setUsingAnchors)
        .addFunction("getUILayoutComponent", &Image::getComponent<UILayoutComponent>)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Container, UILayout>("Container")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("type", &Container::getType, &Container::setType)
        .addFunction("setBoxExpand", &Container::setBoxExpand)
        .addFunction("isBoxExpand", &Container::isBoxExpand)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Polygon, UILayout>("Polygon")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addVertex", 
            luabridge::overload<Vector3>(&Polygon::addVertex),
            luabridge::overload<float, float>(&Polygon::addVertex))
        .addProperty("color", &Polygon::getColor, (void(Polygon::*)(Vector4))&Polygon::setColor)
        .addFunction("setColor", (void(Polygon::*)(float, float, float, float))&Polygon::setColor)
        .addFunction("setTexture", 
            luabridge::overload<std::string>(&Polygon::setTexture),
            luabridge::overload<Framebuffer*>(&Polygon::setTexture))
        .addProperty("flipY", &Polygon::isFlipY, &Polygon::setFlipY)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Text, UILayout>("Text")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setFixedSize", &Text::setFixedSize)
        .addProperty("fixedWidth", &Text::isFixedWidth, &Text::setFixedWidth)
        .addProperty("fixedHeight", &Text::isFixedHeight, &Text::setFixedHeight)
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
        .addProperty("flipY", &Text::isFlipY, &Text::setFlipY)
        .addProperty("pivotBaseline", &Text::isPivotBaseline, &Text::setPivotBaseline)
        .addProperty("pivotCentered", &Text::isPivotCentered, &Text::setPivotCentered)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Image, UILayout>("Image")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setPatchMargin", 
            luabridge::overload<int>(&Image::setPatchMargin),
            luabridge::overload<int, int, int, int>(&Image::setPatchMargin))
        .addProperty("patchMarginBottom", &Image::getPatchMarginBottom, &Image::setPatchMarginBottom)
        .addProperty("patchMarginLeft", &Image::getPatchMarginLeft, &Image::setPatchMarginLeft)
        .addProperty("patchMarginRight", &Image::getPatchMarginRight, &Image::setPatchMarginRight)
        .addProperty("patchMarginTop", &Image::getPatchMarginTop, &Image::setPatchMarginTop)
        .addFunction("setTexture", 
            luabridge::overload<std::string>(&Image::setTexture),
            luabridge::overload<Framebuffer*>(&Image::setTexture))
        .addProperty("color", &Image::getColor, (void(Image::*)(Vector4))&Image::setColor)
        .addFunction("setColor", (void(Image::*)(const float, const float, const float, const float))&Image::setColor)
        .addProperty("flipY", &Image::isFlipY, &Image::setFlipY)
        .addFunction("getUIComponent", &Image::getComponent<UIComponent>)
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
        .addFunction("getButtonComponent", &Button::getComponent<ButtonComponent>)
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

    luabridge::getGlobalNamespace(L)
        .deriveClass<Audio, Object>("Audio")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("loadAudio", &Audio::loadAudio)
        .addFunction("destroyAudio", &Audio::destroyAudio)
        .addFunction("play", &Audio::play)
        .addFunction("pause", &Audio::pause)
        .addFunction("stop", &Audio::stop)
        .addFunction("seek", &Audio::seek)
        .addFunction("getLength", &Audio::getLength)
        .addFunction("getPlayingTime", &Audio::getPlayingTime)
        .addFunction("isPlaying", &Audio::isPlaying)
        .addFunction("isPaused", &Audio::isPaused)
        .addFunction("isStopped", &Audio::isStopped)
        .addProperty("sound3D", &Audio::isSound3D, &Audio::setSound3D)
        .addProperty("clockedSound", &Audio::isClockedSound, &Audio::setClockedSound)
        .addProperty("volume", &Audio::getVolume, &Audio::setVolume)
        .addProperty("speed", &Audio::getSpeed, &Audio::setSpeed)
        .addProperty("pan", &Audio::getPan, &Audio::setPan)
        .addProperty("lopping", &Audio::isLopping, &Audio::setLopping)
        .addProperty("loopingPoint", &Audio::getLoopingPoint, &Audio::setLoopingPoint)
        .addProperty("protectVoice", &Audio::isProtectVoice, &Audio::setProtectVoice)
        .addFunction("setInaudibleBehavior", &Audio::setInaudibleBehavior)
        .addFunction("setMinMaxDistance", &Audio::setMinMaxDistance)
        .addProperty("minDistance", &Audio::getMinDistance, &Audio::setMinDistance)
        .addProperty("maxDistance", &Audio::getMaxDistance, &Audio::setMaxDistance)
        .addProperty("attenuationModel", &Audio::getAttenuationModel, &Audio::setAttenuationModel)
        .addProperty("attenuationRolloffFactor", &Audio::getAttenuationRolloffFactor, &Audio::setAttenuationRolloffFactor)
        .addProperty("dopplerFactor", &Audio::getDopplerFactor, &Audio::setDopplerFactor)
        .addFunction("getAudioComponent", &Image::getComponent<AudioComponent>)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}