//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"
#include "LuaBridgeAddon.h"

#include "EntityHandle.h"
#include "Fog.h"
#include "SkyBox.h"
#include "Object.h"
#include "Camera.h"
#include "Polygon.h"
#include "Terrain.h"
#include "Light.h"
#include "Mesh.h"
#include "Shape.h"
#include "Tilemap.h"
#include "Bone.h"
#include "Model.h"
#include "MeshPolygon.h"
#include "Particles.h"
#include "Sprite.h"
#include "Text.h"
#include "Image.h"
#include "Button.h"
#include "TextEdit.h"
#include "Container.h"
#include "Audio.h"
#include "Body2D.h"
#include "Joint2D.h"
#include "Contact2D.h"
#include "ContactImpulse2D.h"
#include "Manifold2D.h"
#include "WorldManifold2D.h"

using namespace Supernova;


void LuaBinding::registerObjectClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

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
        .beginNamespace("CollisionShape2DType")
        .addProperty("POLYGON", CollisionShape2DType::POLYGON)
        .addProperty("CIRCLE", CollisionShape2DType::CIRCLE)
        .addProperty("EDGE", CollisionShape2DType::EDGE)
        .addProperty("CHAIN", CollisionShape2DType::CHAIN)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Body2DType")
        .addProperty("STATIC", Body2DType::STATIC)
        .addProperty("KINEMATIC", Body2DType::KINEMATIC)
        .addProperty("DYNAMIC", Body2DType::DYNAMIC)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Joint2DType")
        .addProperty("DISTANCE", Joint2DType::DISTANCE)
        .addProperty("REVOLUTE", Joint2DType::REVOLUTE)
        .addProperty("PRISMATIC", Joint2DType::PRISMATIC)
        .addProperty("PULLEY", Joint2DType::PULLEY)
        .addProperty("GEAR", Joint2DType::GEAR)
        .addProperty("MOUSE", Joint2DType::MOUSE)
        .addProperty("WHEEL", Joint2DType::WHEEL)
        .addProperty("WELD", Joint2DType::WELD)
        .addProperty("FRICTION", Joint2DType::FRICTION)
        .addProperty("MOTOR", Joint2DType::MOTOR)
        .addProperty("ROPE", Joint2DType::ROPE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Manifold2DType")
        .addProperty("CIRCLES", Manifold2DType::CIRCLES)
        .addProperty("FACEA", Manifold2DType::FACEA)
        .addProperty("FACEB", Manifold2DType::FACEB)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<EntityHandle>("EntityHandle")
        .addConstructor <void (Scene*)> ()
        .addProperty("entity", &EntityHandle::getEntity)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Fog, EntityHandle>("Fog")
        .addConstructor <void (Scene*)> ()
        .addProperty("type", &Fog::getType, &Fog::setType)
        .addProperty("color", &Fog::getColor, (void(Fog::*)(Vector3))&Fog::setColor)
        .addFunction("setColor", (void(Fog::*)(const float, const float, const float))&Fog::setColor)
        .addProperty("density", &Fog::getDensity, &Fog::setDensity)
        .addProperty("linearStart", &Fog::getLinearStart, &Fog::setLinearStart)
        .addProperty("linearEnd", &Fog::getLinearEnd, &Fog::setLinearEnd)
        .addFunction("setLinearStartEnd", &Fog::setLinearStartEnd)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<SkyBox, EntityHandle>("SkyBox")
        .addConstructor <void (Scene*)> ()
        .addFunction("setTextures", &SkyBox::setTextures)
        .addFunction("setTextureFront", &SkyBox::setTextureFront)
        .addFunction("setTextureBack", &SkyBox::setTextureBack)
        .addFunction("setTextureLeft", &SkyBox::setTextureLeft)
        .addFunction("setTextureRight", &SkyBox::setTextureRight)
        .addFunction("setTextureUp", &SkyBox::setTextureUp)
        .addFunction("setTextureDown", &SkyBox::setTextureDown)
        .addProperty("color", &SkyBox::getColor, (void(SkyBox::*)(Vector4))&SkyBox::setColor)
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&SkyBox::setColor),
            luabridge::overload<const float, const float, const float, const float>(&SkyBox::setColor))
        .addProperty("alpha", &SkyBox::getAlpha, &SkyBox::setAlpha)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Object, EntityHandle>("Object")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addChild", 
            luabridge::overload<Object*>(&Object::addChild),
            luabridge::overload<Entity>(&Object::addChild))
        .addFunction("moveToFirst", &Object::moveToFirst)
        .addFunction("moveUp", &Object::moveUp)
        .addFunction("moveDown", &Object::moveDown)
        .addFunction("moveToLast", &Object::moveToLast)
        .addProperty("name", &Object::getName, &Object::setName)
        .addProperty("position", &Object::getPosition, (void(Object::*)(Vector3))&Object::setPosition)
        .addFunction("setPosition", 
            luabridge::overload<const float, const float, const float>(&Object::setPosition),
            luabridge::overload<const float, const float>(&Object::setPosition))
        .addFunction("getWorldPosition", &Object::getWorldPosition)
        .addProperty("rotation", &Object::getRotation, (void(Object::*)(Quaternion))&Object::setRotation)
        .addFunction("setRotation", (void(Object::*)(const float, const float, const float))&Object::setRotation)
        .addFunction("getWorldRotation", &Object::getWorldRotation)
        .addProperty("scale", &Object::getScale, (void(Object::*)(Vector3))&Object::setScale)
        .addFunction("setScale", (void(Object::*)(const float))&Object::setScale)
        .addFunction("getWorldScale", &Object::getWorldScale)
        .addProperty("visible", &Object::isVisible, &Object::setVisible)
        .addFunction("setBillboard", (void(Object::*)(bool, bool, bool))&Object::setBillboard)
        .addProperty("billboard", &Object::isBillboard, (void(Object::*)(bool))&Object::setBillboard)
        .addProperty("fakeBillboard", &Object::isFakeBillboard, &Object::setFakeBillboard)
        .addProperty("cylindricalBillboard", &Object::isCylindricalBillboard, &Object::setCylindricalBillboard)
        .addFunction("setModelMatrix", &Object::setModelMatrix)
        .addFunction("updateTransform", &Object::updateTransform)
        .addFunction("getBody2D", &Object::getBody2D)
        .addFunction("removeBody2D", &Object::removeBody2D)
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
        .addProperty("transparentSort", &Camera::isTransparentSort, &Camera::setTransparentSort)
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
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&Mesh::setColor),
            luabridge::overload<const float, const float, const float, const float>(&Mesh::setColor))
        .addProperty("alpha", &Mesh::getAlpha, &Mesh::setAlpha)
        .addFunction("getMaterial", &Mesh::getMaterial)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Shape, Mesh>("Shape")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("createPlane", 
            luabridge::overload<float, float>(&Shape::createPlane),
            luabridge::overload<float, float, unsigned int>(&Shape::createPlane))
        .addFunction("createBox", 
            luabridge::overload<float, float, float>(&Shape::createBox),
            luabridge::overload<float, float, float, unsigned int>(&Shape::createBox))
        .addFunction("createSphere", 
            luabridge::overload<float>(&Shape::createSphere),
            luabridge::overload<float, unsigned int, unsigned int>(&Shape::createSphere))
        .addFunction("createCylinder", 
            luabridge::overload<float, float>(&Shape::createCylinder),
            luabridge::overload<float, float, float>(&Shape::createCylinder),
            luabridge::overload<float, float, unsigned int, unsigned int>(&Shape::createCylinder),
            luabridge::overload<float, float, float, unsigned int, unsigned int>(&Shape::createCylinder))
        .addFunction("createTorus", 
            luabridge::overload<float, float>(&Shape::createTorus),
            luabridge::overload<float, float, unsigned int, unsigned int>(&Shape::createTorus))
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
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&Terrain::setColor),
            luabridge::overload<const float, const float, const float, const float>(&Terrain::setColor))
        .addProperty("alpha", &Terrain::getAlpha, &Terrain::setAlpha)
        .addProperty("size", &Terrain::getSize, &Terrain::setSize)
        .addProperty("maxHeight", &Terrain::getMaxHeight, &Terrain::setMaxHeight)
        .addProperty("resolution", &Terrain::getResolution, &Terrain::setResolution)
        .addProperty("textureBaseTiles", &Terrain::getTextureBaseTiles, &Terrain::setTextureBaseTiles)
        .addProperty("textureDetailTiles", &Terrain::getTextureDetailTiles, &Terrain::setTextureDetailTiles)
        .addProperty("rootGridSize", &Terrain::getRootGridSize, &Terrain::setRootGridSize)
        .addProperty("levels", &Terrain::getLevels, &Terrain::setLevels)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Bone, Object>("Bone")
        .addConstructor <void (*) (Scene*, Entity)> ()
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Tilemap, Mesh>("Tilemap")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("findRectByString", &Tilemap::findRectByString)
        .addFunction("findTileByString", &Tilemap::findTileByString)
        .addFunction("addRect", 
            luabridge::overload<int, std::string, std::string, TextureFilter, Rect>(&Tilemap::addRect),
            luabridge::overload<int, std::string, std::string, Rect>(&Tilemap::addRect),
            luabridge::overload<int, std::string, Rect>(&Tilemap::addRect),
            luabridge::overload<std::string, float, float, float, float>(&Tilemap::addRect),
            luabridge::overload<float, float, float, float>(&Tilemap::addRect),
            luabridge::overload<Rect>(&Tilemap::addRect))
        .addFunction("removeRect", 
            luabridge::overload<int>(&Tilemap::removeRect),
            luabridge::overload<std::string>(&Tilemap::removeRect))
        .addFunction("clearRects", &Tilemap::clearRects)
        .addFunction("getRect", 
            luabridge::overload<int>(&Tilemap::getRect),
            luabridge::overload<std::string>(&Tilemap::getRect))
        .addFunction("addTile", 
            luabridge::overload<int, std::string, int, Vector2, float, float>(&Tilemap::addTile),
            luabridge::overload<std::string, int, Vector2, float, float>(&Tilemap::addTile),
            luabridge::overload<int, Vector2, float, float>(&Tilemap::addTile),
            luabridge::overload<std::string, std::string, Vector2, float, float>(&Tilemap::addTile),
            luabridge::overload<std::string, Vector2, float, float>(&Tilemap::addTile))
        .addFunction("removeTile", 
            luabridge::overload<int>(&Tilemap::removeTile),
            luabridge::overload<std::string>(&Tilemap::removeTile))
        .addFunction("clearTiles", &Tilemap::clearTiles)
        .addFunction("getTile", 
            luabridge::overload<int>(&Tilemap::getTile),
            luabridge::overload<std::string>(&Tilemap::getTile))
        .addProperty("reserveTiles", &Tilemap::getReserveTiles, &Tilemap::setReserveTiles)
        .addFunction("clearAll", &Tilemap::clearAll)
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
        .deriveClass<Sprite, Mesh>("Sprite")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &Sprite::setSize)
        .addProperty("width", &Sprite::getWidth, &Sprite::setWidth)
        .addProperty("height", &Sprite::getHeight, &Sprite::setHeight)
        .addProperty("flipY", &Sprite::isFlipY, &Sprite::setFlipY)
        .addProperty("textureRect", &Sprite::getTextureRect, (void(Sprite::*)(Rect))&Sprite::setTextureRect)
        .addFunction("setTextureRect",(void(Sprite::*)(float, float, float, float)) &Sprite::setTextureRect)
        .addProperty("pivotPreset", &Sprite::getPivotPreset, &Sprite::setPivotPreset)
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
            luabridge::overload<int, int, int, bool>(&Sprite::startAnimation),
            luabridge::overload<std::string, int, bool>(&Sprite::startAnimation))
        .addFunction("pauseAnimation", &Sprite::pauseAnimation)
        .addFunction("stopAnimation", &Sprite::stopAnimation)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<UILayout, Object>("UILayout")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &UILayout::setSize)
        .addProperty("width", &UILayout::getWidth, &Image::setWidth)
        .addProperty("height", &UILayout::getHeight, &UILayout::setHeight)
        .addFunction("setAnchorPoints", &UILayout::setAnchorPoints)
        .addProperty("anchorPointLeft", &UILayout::getAnchorPointLeft, &UILayout::setAnchorPointLeft)
        .addProperty("anchorPointTop", &UILayout::getAnchorPointTop, &UILayout::setAnchorPointTop)
        .addProperty("anchorPointRight", &UILayout::getAnchorPointRight, &UILayout::setAnchorPointRight)
        .addProperty("anchorPointBottom", &UILayout::getAnchorPointBottom, &UILayout::setAnchorPointBottom)
        .addFunction("setAnchorOffsets", &UILayout::setAnchorOffsets)
        .addProperty("anchorOffsetLeft", &UILayout::getAnchorOffsetLeft, &UILayout::setAnchorOffsetLeft)
        .addProperty("anchorOffsetTop", &UILayout::getAnchorOffsetTop, &UILayout::setAnchorOffsetTop)
        .addProperty("anchorOffsetRight", &UILayout::getAnchorOffsetRight, &UILayout::setAnchorOffsetRight)
        .addProperty("anchorOffsetBottom", &UILayout::getAnchorOffsetBottom, &UILayout::setAnchorOffsetBottom)
        .addProperty("anchorPreset", &UILayout::getAnchorPreset, &UILayout::setAnchorPreset)
        .addProperty("usingAnchors", &UILayout::isUsingAnchors, &UILayout::setUsingAnchors)
        .addProperty("ignoreScissor", &UILayout::isIgnoreScissor, &UILayout::setIgnoreScissor)
        .addFunction("getUILayoutComponent", &Image::getComponent<UILayoutComponent>)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Container, UILayout>("Container")
        .addConstructor <void (*) (Scene*)> ()
        .addProperty("type", &Container::getType, &Container::setType)
        .addFunction("resize", &Container::resize)
        .addFunction("setBoxExpand", 
            luabridge::overload<size_t, bool>(&Container::setBoxExpand),
            luabridge::overload<bool>(&Container::setBoxExpand))
        .addFunction("isBoxExpand", &Container::isBoxExpand)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Polygon, UILayout>("Polygon")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addVertex", 
            luabridge::overload<Vector3>(&Polygon::addVertex),
            luabridge::overload<float, float>(&Polygon::addVertex))
        .addProperty("color", &Polygon::getColor, (void(Polygon::*)(Vector4))&Polygon::setColor)
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&Polygon::setColor),
            luabridge::overload<const float, const float, const float, const float>(&Polygon::setColor))
        .addProperty("alpha", &Polygon::getAlpha, &Polygon::setAlpha)
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
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&Text::setColor),
            luabridge::overload<const float, const float, const float, const float>(&Text::setColor))
        .addProperty("alpha", &Text::getAlpha, &Text::setAlpha)
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
        .addFunction("setColor", 
            luabridge::overload<const float, const float, const float>(&Image::setColor),
            luabridge::overload<const float, const float, const float, const float>(&Image::setColor))
        .addProperty("alpha", &Image::getAlpha, &Image::setAlpha)
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

    luabridge::getGlobalNamespace(L)
        .deriveClass<Body2D, EntityHandle>("Body2D")
        .addConstructor <void (Scene*, Entity)> ()
        .addFunction("getAttachedObject", &Body2D::getAttachedObject)
        .addFunction("createRectShape", &Body2D::createRectShape)
        .addFunction("createPolygonShape2D", &Body2D::createPolygonShape2D)
        .addFunction("createCircleShape2D", &Body2D::createCircleShape2D)
        .addFunction("createTwoSidedEdgeShape2D", &Body2D::createTwoSidedEdgeShape2D)
        .addFunction("createOneSidedEdgeShape2D", &Body2D::createOneSidedEdgeShape2D)
        .addFunction("createLoopChainShape2D", &Body2D::createLoopChainShape2D)
        .addFunction("createChainShape2D", &Body2D::createChainShape2D)
        .addFunction("removeAllShapes", &Body2D::removeAllShapes)
        .addProperty("shapeDensity", &Body2D::getShapeDensity, &Body2D::setShapeDensity)
        .addFunction("setShapeDensity", 
            luabridge::overload<float>(&Body2D::setShapeDensity),
            luabridge::overload<size_t, float>(&Body2D::setShapeDensity))
        .addFunction("getShapeDensity", 
            luabridge::overload<>(&Body2D::getShapeDensity),
            luabridge::overload<size_t>(&Body2D::getShapeDensity))
        .addProperty("shapeFriction", &Body2D::getShapeFriction, &Body2D::setShapeFriction)
        .addFunction("setShapeFriction", 
            luabridge::overload<float>(&Body2D::setShapeFriction),
            luabridge::overload<size_t, float>(&Body2D::setShapeFriction))
        .addFunction("getShapeFriction", 
            luabridge::overload<>(&Body2D::getShapeFriction),
            luabridge::overload<size_t>(&Body2D::getShapeFriction))
        .addProperty("shapeRestitution", &Body2D::getShapeRestitution, &Body2D::setShapeRestitution)
        .addFunction("setShapeRestitution", 
            luabridge::overload<float>(&Body2D::setShapeRestitution),
            luabridge::overload<size_t, float>(&Body2D::setShapeRestitution))
        .addFunction("getShapeRestitution", 
            luabridge::overload<>(&Body2D::getShapeRestitution),
            luabridge::overload<size_t>(&Body2D::getShapeRestitution))
        .addProperty("linearVelocity", &Body2D::getLinearVelocity, &Body2D::setLinearVelocity)
        .addProperty("angularVelocity", &Body2D::getAngularVelocity, &Body2D::setAngularVelocity)
        .addProperty("linearDamping", &Body2D::getLinearDamping, &Body2D::setLinearDamping)
        .addProperty("angularDamping", &Body2D::getAngularDamping, &Body2D::setAngularDamping)
        .addProperty("allowSleep", &Body2D::isAllowSleep, &Body2D::setAllowSleep)
        .addProperty("awake", &Body2D::isAwake, &Body2D::setAwake)
        .addProperty("fixedRotation", &Body2D::isFixedRotation, &Body2D::setFixedRotation)
        .addProperty("bullet", &Body2D::isBullet, &Body2D::setBullet)
        .addProperty("type", &Body2D::getType, &Body2D::setType)
        .addProperty("enabled", &Body2D::isEnabled, &Body2D::setEnabled)
        .addProperty("gravityScale", &Body2D::getGravityScale, &Body2D::setGravityScale)
        .addProperty("categoryBitsFilter", &Body2D::getCategoryBitsFilter, &Body2D::setCategoryBitsFilter)
        .addFunction("setCategoryBitsFilter", 
            luabridge::overload<uint16_t>(&Body2D::setCategoryBitsFilter),
            luabridge::overload<size_t, uint16_t>(&Body2D::setCategoryBitsFilter))
        .addFunction("getCategoryBitsFilter", 
            luabridge::overload<>(&Body2D::getCategoryBitsFilter),
            luabridge::overload<size_t>(&Body2D::getCategoryBitsFilter))
        .addProperty("maskBitsFilter", &Body2D::getMaskBitsFilter, &Body2D::setMaskBitsFilter)
        .addFunction("setMaskBitsFilter", 
            luabridge::overload<uint16_t>(&Body2D::setMaskBitsFilter),
            luabridge::overload<size_t, uint16_t>(&Body2D::setMaskBitsFilter))
        .addFunction("getMaskBitsFilter", 
            luabridge::overload<>(&Body2D::getMaskBitsFilter),
            luabridge::overload<size_t>(&Body2D::getMaskBitsFilter))
        .addProperty("groupIndexFilter", &Body2D::getGroupIndexFilter, &Body2D::setGroupIndexFilter)
        .addFunction("setGroupIndexFilter", 
            luabridge::overload<int16_t>(&Body2D::setGroupIndexFilter),
            luabridge::overload<size_t, int16_t>(&Body2D::setGroupIndexFilter))
        .addFunction("getGroupIndexFilter", 
            luabridge::overload<>(&Body2D::getGroupIndexFilter),
            luabridge::overload<size_t>(&Body2D::getGroupIndexFilter))
        .addFunction("getMass", &Body2D::getMass)
        .addFunction("getInertia", &Body2D::getInertia)
        .addFunction("getLinearVelocityFromWorldPoint", &Body2D::getLinearVelocityFromWorldPoint)
        .addFunction("applyForce", &Body2D::applyForce)
        .addFunction("applyForceToCenter", &Body2D::applyForceToCenter)
        .addFunction("applyTorque", &Body2D::applyTorque)
        .addFunction("applyLinearImpulse", &Body2D::applyLinearImpulse)
        .addFunction("applyLinearImpulseToCenter", &Body2D::applyLinearImpulseToCenter)
        .addFunction("applyAngularImpulse", &Body2D::applyAngularImpulse)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Joint2D>("Joint2D")
        .addFunction("setDistanceJoint", &Joint2D::setDistanceJoint)
        .addFunction("setRevoluteJoint", &Joint2D::setRevoluteJoint)
        .addFunction("setPrismaticJoint", &Joint2D::setPrismaticJoint)
        .addFunction("setPulleyJoint", &Joint2D::setPulleyJoint)
        .addFunction("setGearJoint", &Joint2D::setGearJoint)
        .addFunction("setMouseJoint", &Joint2D::setMouseJoint)
        .addFunction("setWheelJoint", &Joint2D::setWheelJoint)
        .addFunction("setWeldJoint", &Joint2D::setWeldJoint)
        .addFunction("setFrictionJoint", &Joint2D::setFrictionJoint)
        .addFunction("setMotorJoint", &Joint2D::setMotorJoint)
        .addFunction("setRopeJoint", &Joint2D::setRopeJoint)
        .addFunction("getType", &Joint2D::getType)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Manifold2D>("Manifold2D")
        .addFunction("getManifoldPointLocalPoint", &Manifold2D::getManifoldPointLocalPoint)
        .addFunction("getManifoldPointNormalImpulse", &Manifold2D::getManifoldPointNormalImpulse)
        .addFunction("getManifoldPointTangentImpulse", &Manifold2D::getManifoldPointTangentImpulse)
        .addFunction("getLocalNormal", &Manifold2D::getLocalNormal)
        .addFunction("getLocalPoint", &Manifold2D::getLocalPoint)
        .addFunction("getType", &Manifold2D::getType)
        .addFunction("getPointCount", &Manifold2D::getPointCount)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<WorldManifold2D>("WorldManifold2D")
        .addFunction("getNormal", &WorldManifold2D::getNormal)
        .addFunction("getPoint", &WorldManifold2D::getPoint)
        .addFunction("getSeparations", &WorldManifold2D::getSeparations)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Contact2D>("Contact2D")
        .addFunction("getManifold", &Contact2D::getManifold)
        .addFunction("getWorldManifold", &Contact2D::getWorldManifold)
        .addFunction("isTouching", &Contact2D::isTouching)
        .addFunction("getBodyEntityA", &Contact2D::getBodyEntityA)
        .addFunction("getBodyA", &Contact2D::getBodyA)
        .addFunction("getShapeIndexA", &Contact2D::getShapeIndexA)
        .addFunction("getBodyEntityB", &Contact2D::getBodyEntityB)
        .addFunction("getBodyB", &Contact2D::getBodyB)
        .addFunction("getShapeIndexB", &Contact2D::getShapeIndexB)
        .addProperty("enabled", &Contact2D::isEnabled, &Contact2D::setEnabled)
        .addProperty("friction", &Contact2D::getFriction, &Contact2D::setFriction)
        .addFunction("resetFriction", &Contact2D::resetFriction)
        .addProperty("restitution", &Contact2D::getRestitution, &Contact2D::setRestitution)
        .addFunction("resetRestitution", &Contact2D::resetRestitution)
        .addProperty("tangentSpeed", &Contact2D::getTangentSpeed, &Contact2D::setTangentSpeed)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<ContactImpulse2D>("ContactImpulse2D")
        .addFunction("getCount", &ContactImpulse2D::getCount)
        .addFunction("getNormalImpulses", &ContactImpulse2D::getNormalImpulses)
        .addFunction("getTangentImpulses", &ContactImpulse2D::getTangentImpulses)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}