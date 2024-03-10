//
// (c) 2024 Eduardo Doria.
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
#include "Lines.h"
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
#include "Body3D.h"
#include "Joint3D.h"
#include "Contact3D.h"
#include "CollideShapeResult3D.h"

using namespace Supernova;


void LuaBinding::registerObjectClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("CameraType")
        .addVariable("CAMERA_2D", CameraType::CAMERA_2D)
        .addVariable("CAMERA_ORTHO", CameraType::CAMERA_ORTHO)
        .addVariable("CAMERA_PERSPECTIVE", CameraType::CAMERA_PERSPECTIVE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("FrustumPlane")
        .addVariable("FRUSTUM_PLANE_NEAR", FrustumPlane::FRUSTUM_PLANE_NEAR)
        .addVariable("FRUSTUM_PLANE_FAR", FrustumPlane::FRUSTUM_PLANE_FAR)
        .addVariable("FRUSTUM_PLANE_LEFT", FrustumPlane::FRUSTUM_PLANE_LEFT)
        .addVariable("FRUSTUM_PLANE_RIGHT", FrustumPlane::FRUSTUM_PLANE_RIGHT)
        .addVariable("FRUSTUM_PLANE_TOP", FrustumPlane::FRUSTUM_PLANE_TOP)
        .addVariable("FRUSTUM_PLANE_BOTTOM", FrustumPlane::FRUSTUM_PLANE_BOTTOM)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("LightType")
        .addVariable("DIRECTIONAL", LightType::DIRECTIONAL)
        .addVariable("POINT", LightType::POINT)
        .addVariable("SPOT", LightType::SPOT)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("CollisionShape2DType")
        .addVariable("POLYGON", CollisionShape2DType::POLYGON)
        .addVariable("CIRCLE", CollisionShape2DType::CIRCLE)
        .addVariable("EDGE", CollisionShape2DType::EDGE)
        .addVariable("CHAIN", CollisionShape2DType::CHAIN)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("CollisionShape3DType")
        .addVariable("SPHERE", CollisionShape3DType::SPHERE)
        .addVariable("BOX", CollisionShape3DType::BOX)
        .addVariable("CAPSULE", CollisionShape3DType::CAPSULE)
        .addVariable("TAPERED_CAPSULE", CollisionShape3DType::TAPERED_CAPSULE)
        .addVariable("CYLINDER", CollisionShape3DType::CYLINDER)
        .addVariable("CONVEX_HULL", CollisionShape3DType::CONVEX_HULL)
        .addVariable("MESH", CollisionShape3DType::MESH)
        .addVariable("HEIGHTFIELD", CollisionShape3DType::HEIGHTFIELD)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("BodyType")
        .addVariable("STATIC", BodyType::STATIC)
        .addVariable("KINEMATIC", BodyType::KINEMATIC)
        .addVariable("DYNAMIC", BodyType::DYNAMIC)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Joint2DType")
        .addVariable("DISTANCE", Joint2DType::DISTANCE)
        .addVariable("REVOLUTE", Joint2DType::REVOLUTE)
        .addVariable("PRISMATIC", Joint2DType::PRISMATIC)
        .addVariable("PULLEY", Joint2DType::PULLEY)
        .addVariable("GEAR", Joint2DType::GEAR)
        .addVariable("MOUSE", Joint2DType::MOUSE)
        .addVariable("WHEEL", Joint2DType::WHEEL)
        .addVariable("WELD", Joint2DType::WELD)
        .addVariable("FRICTION", Joint2DType::FRICTION)
        .addVariable("MOTOR", Joint2DType::MOTOR)
        .addVariable("ROPE", Joint2DType::ROPE)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Joint3DType")
        .addVariable("FIXED", Joint3DType::FIXED)
        .addVariable("DISTANCE", Joint3DType::DISTANCE)
        .addVariable("POINT", Joint3DType::POINT)
        .addVariable("HINGE", Joint3DType::HINGE)
        .addVariable("CONE", Joint3DType::CONE)
        .addVariable("PRISMATIC", Joint3DType::PRISMATIC)
        .addVariable("SWINGTWIST", Joint3DType::SWINGTWIST)
        .addVariable("SIXDOF", Joint3DType::SIXDOF)
        .addVariable("PATH", Joint3DType::PATH)
        .addVariable("GEAR", Joint3DType::GEAR)
        .addVariable("RACKANDPINON", Joint3DType::RACKANDPINON)
        .addVariable("PULLEY", Joint3DType::PULLEY)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Manifold2DType")
        .addVariable("CIRCLES", Manifold2DType::CIRCLES)
        .addVariable("FACEA", Manifold2DType::FACEA)
        .addVariable("FACEB", Manifold2DType::FACEB)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<EntityHandle>("EntityHandle")
        .addConstructor <void (Scene*)> ()
        .addProperty("scene", &EntityHandle::getScene)
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
        .addProperty("modelMatrix", &Object::getModelMatrix, &Object::setModelMatrix)
        .addProperty("normalMatrix", &Object::getNormalMatrix)
        .addProperty("modelViewProjectionMatrix", &Object::getModelViewProjectionMatrix)
        .addFunction("updateTransform", &Object::updateTransform)
        .addFunction("getBody2D", &Object::getBody2D)
        .addFunction("removeBody2D", &Object::removeBody2D)
        .addFunction("getBody3D", &Object::getBody3D)
        .addFunction("removeBody3D", &Object::removeBody3D)
        .addFunction("getRay", &Object::getRay)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Camera, Object>("Camera")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("activate", &Camera::activate)
        .addFunction("setOrtho", &Camera::setOrtho)
        .addFunction("setPerspective", &Camera::setPerspective)
        .addProperty("near",  &Camera::getNear, &Camera::setNear)
        .addProperty("far",  &Camera::getFar, &Camera::setFar)
        .addProperty("left",  &Camera::getLeft, &Camera::setLeft)
        .addProperty("right",  &Camera::getRight, &Camera::setRight)
        .addProperty("bottom",  &Camera::getBottom, &Camera::setBottom)
        .addProperty("top",  &Camera::getTop, &Camera::setTop)
        .addProperty("aspect",  &Camera::getAspect, &Camera::setAspect)
        .addProperty("yfov",  &Camera::getYFov, &Camera::setYFov)
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
        .addFunction("screenToRay", &Camera::screenToRay)
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
        .addProperty("primitiveType", &Mesh::getPrimitiveType, &Mesh::setPrimitiveType)
        .addFunction("setPrimitiveType", 
            luabridge::overload<PrimitiveType>(&Mesh::setPrimitiveType),
            luabridge::overload<unsigned int, PrimitiveType>(&Mesh::setPrimitiveType))
        .addFunction("getPrimitiveType", 
            luabridge::overload<>(&Mesh::getPrimitiveType),
            luabridge::overload<unsigned int>(&Mesh::getPrimitiveType))
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
        .addFunction("createCapsule", 
            luabridge::overload<float, float>(&Shape::createCapsule),
            luabridge::overload<float, float, float>(&Shape::createCapsule),
            luabridge::overload<float, float, unsigned int, unsigned int>(&Shape::createCapsule),
            luabridge::overload<float, float, float, unsigned int, unsigned int>(&Shape::createCapsule))
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
        .addProperty("textureCutFactor", &Tilemap::getTextureCutFactor, &Tilemap::setTextureCutFactor)
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
        .deriveClass<Lines, Object>("Lines")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("addLine", 
            luabridge::overload<Vector3, Vector3>(&Lines::addLine),
            luabridge::overload<Vector3, Vector3, Vector3>(&Lines::addLine),
            luabridge::overload<Vector3, Vector3, Vector4>(&Lines::addLine),
            luabridge::overload<Vector3, Vector3, Vector4, Vector4>(&Lines::addLine))
        .addFunction("getLine", &Lines::getLine)
        .addFunction("setLine", &Lines::setLine)
        .addFunction("setLinePointA", &Lines::setLinePointA)
        .addFunction("setLinePointB", &Lines::setLinePointB)
        .addFunction("setLineColorA", &Lines::setLineColorA)
        .addFunction("setLineColorB", &Lines::setLineColorB)
        .addFunction("setLineColor", 
            luabridge::overload<size_t, Vector3>(&Lines::setLineColor),
            luabridge::overload<size_t, Vector4>(&Lines::setLineColor))
        .addFunction("clearLines", &Lines::clearLines)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Sprite, Mesh>("Sprite")
        .addConstructor <void (*) (Scene*)> ()
        .addFunction("setSize", &Sprite::setSize)
        .addProperty("width", &Sprite::getWidth, &Sprite::setWidth)
        .addProperty("height", &Sprite::getHeight, &Sprite::setHeight)
        .addProperty("flipY", &Sprite::isFlipY, &Sprite::setFlipY)
        .addProperty("textureCutFactor", &Sprite::getTextureCutFactor, &Sprite::setTextureCutFactor)
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
        .addProperty("width", &UILayout::getWidth, &UILayout::setWidth)
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
        .addFunction("getUILayoutComponent", &UILayout::getComponent<UILayoutComponent>)
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
        .addProperty("textureCutFactor", &Image::getTextureCutFactor, &Image::setTextureCutFactor)
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
        .addProperty("pointsToMeterScale", &Body2D::getPointsToMeterScale)
        .addFunction("load", &Body2D::load)
        .addFunction("createRectShape", &Body2D::createRectShape)
        .addFunction("createCenteredRectShape", 
            luabridge::overload<float, float>(&Body2D::createCenteredRectShape),
            luabridge::overload<float, float, Vector2, float>(&Body2D::createCenteredRectShape))
        .addFunction("createPolygonShape", &Body2D::createPolygonShape)
        .addFunction("createCircleShape", &Body2D::createCircleShape)
        .addFunction("createTwoSidedEdgeShape", &Body2D::createTwoSidedEdgeShape)
        .addFunction("createOneSidedEdgeShape", &Body2D::createOneSidedEdgeShape)
        .addFunction("createLoopChainShape", &Body2D::createLoopChainShape)
        .addFunction("createChainShape", &Body2D::createChainShape)
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
        .addFunction("resetMassData", &Body2D::resetMassData)
        .addFunction("applyForce", &Body2D::applyForce)
        .addFunction("applyForceToCenter", &Body2D::applyForceToCenter)
        .addFunction("applyTorque", &Body2D::applyTorque)
        .addFunction("applyLinearImpulse", &Body2D::applyLinearImpulse)
        .addFunction("applyLinearImpulseToCenter", &Body2D::applyLinearImpulseToCenter)
        .addFunction("applyAngularImpulse", &Body2D::applyAngularImpulse)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Joint2D>("Joint2D")
        .addConstructor <void (Scene*), void (Scene*, Entity)> ()
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

    luabridge::getGlobalNamespace(L)
        .deriveClass<Body3D, EntityHandle>("Body3D")
        .addConstructor <void (Scene*, Entity)> ()
        .addFunction("getAttachedObject", &Body3D::getAttachedObject)
        .addFunction("load", &Body3D::load)
        .addFunction("createBoxShape", 
            luabridge::overload<float, float, float>(&Body3D::createBoxShape),
            luabridge::overload<Vector3, Quaternion, float, float, float>(&Body3D::createBoxShape))
        .addFunction("createSphereShape", 
            luabridge::overload<float>(&Body3D::createSphereShape),
            luabridge::overload<Vector3, Quaternion, float>(&Body3D::createSphereShape))
        .addFunction("createCapsuleShape", 
            luabridge::overload<float, float>(&Body3D::createCapsuleShape),
            luabridge::overload<Vector3, Quaternion, float, float>(&Body3D::createCapsuleShape))
        .addFunction("createTaperedCapsuleShape", 
            luabridge::overload<float, float, float>(&Body3D::createTaperedCapsuleShape),
            luabridge::overload<Vector3, Quaternion, float, float, float>(&Body3D::createTaperedCapsuleShape))
        .addFunction("createCylinderShape", 
            luabridge::overload<float, float>(&Body3D::createCylinderShape),
            luabridge::overload<Vector3, Quaternion, float, float>(&Body3D::createCylinderShape))
        .addFunction("createConvexHullShape", 
            luabridge::overload<std::vector<Vector3>>(&Body3D::createConvexHullShape),
            luabridge::overload<Vector3, Quaternion, std::vector<Vector3>>(&Body3D::createConvexHullShape))
        .addFunction("createMeshShape", 
            luabridge::overload<>(&Body3D::createMeshShape),
            luabridge::overload<std::vector<Vector3>, std::vector<uint16_t>>(&Body3D::createMeshShape),
            luabridge::overload<Vector3, Quaternion, std::vector<Vector3>, std::vector<uint16_t>>(&Body3D::createMeshShape))
        .addFunction("createHeightFieldShape", 
            luabridge::overload<>(&Body3D::createHeightFieldShape),
            luabridge::overload<unsigned int>(&Body3D::createHeightFieldShape))
        .addProperty("shapeDensity", &Body3D::getShapeDensity, &Body3D::setShapeDensity)
        .addFunction("setShapeDensity", 
            luabridge::overload<float>(&Body3D::setShapeDensity),
            luabridge::overload<size_t, float>(&Body3D::setShapeDensity))
        .addFunction("getShapeDensity", 
            luabridge::overload<>(&Body3D::getShapeDensity),
            luabridge::overload<size_t>(&Body3D::getShapeDensity))
        .addProperty("type", &Body3D::getType, &Body3D::setType)
        .addFunction("canBeKinematicOrDynamic", &Body3D::canBeKinematicOrDynamic)
        .addFunction("activate", &Body3D::activate)
        .addFunction("deactivate", &Body3D::deactivate)
        .addProperty("sensor", &Body3D::isSensor, &Body3D::setIsSensor)
        .addProperty("sensorDetectsStatic", &Body3D::isSensorDetectsStatic, &Body3D::setSensorDetectsStatic)
        .addFunction("setAllowedDOFsAll", &Body3D::setAllowedDOFsAll)
        .addFunction("setAllowedDOFs2DPlane", &Body3D::setAllowedDOFs2DPlane)
        .addFunction("setAllowedDOFs", &Body3D::setAllowedDOFs)
        .addProperty("mass", &Body3D::getMass, &Body3D::setMass)
        .addProperty("gravityFactor", &Body3D::getGravityFactor, &Body3D::setGravityFactor)
        .addProperty("collisionGroupID", &Body3D::getCollisionGroupID, &Body3D::setCollisionGroupID)
        .addProperty("sllowSleeping", &Body3D::isAllowSleeping, &Body3D::setAllowSleeping)
        .addProperty("friction", &Body3D::getFriction, &Body3D::setFriction)
        .addProperty("restitution", &Body3D::getRestitution, &Body3D::setRestitution)
        .addProperty("linearVelocity", &Body3D::getLinearVelocity, &Body3D::setLinearVelocity)
        .addFunction("setLinearVelocityClamped", &Body3D::setLinearVelocityClamped)
        .addProperty("angularVelocity", &Body3D::getAngularVelocity, &Body3D::setAngularVelocity)
        .addFunction("setAngularVelocityClamped", &Body3D::setAngularVelocityClamped)
        .addFunction("getPointVelocityCOM", &Body3D::getPointVelocityCOM)
        .addFunction("getPointVelocity", &Body3D::getPointVelocity)
        .addFunction("getAccumulatedForce", &Body3D::getAccumulatedForce)
        .addFunction("getAccumulatedTorque", &Body3D::getAccumulatedTorque)
        .addFunction("getInverseInertia", &Body3D::getInverseInertia)
        .addFunction("applyForce", 
            luabridge::overload<const Vector3&>(&Body3D::applyForce),
            luabridge::overload<const Vector3&, const Vector3&>(&Body3D::applyForce))
        .addFunction("applyTorque", &Body3D::applyTorque)
        .addFunction("applyImpulse", 
            luabridge::overload<const Vector3&>(&Body3D::applyImpulse),
            luabridge::overload<const Vector3&, const Vector3&>(&Body3D::applyImpulse))
        .addFunction("applyAngularImpulse", &Body3D::applyAngularImpulse)
        .addFunction("applyBuoyancyImpulse", &Body3D::applyBuoyancyImpulse)
        .addFunction("getCenterOfMassPosition", &Body3D::getCenterOfMassPosition)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Joint3D>("Joint3D")
        .addConstructor <void (Scene*), void (Scene*, Entity)> ()
        .addFunction("setFixedJoint", &Joint3D::setFixedJoint)
        .addFunction("setDistanceJoint", &Joint3D::setDistanceJoint)
        .addFunction("setPointJoint", &Joint3D::setPointJoint)
        .addFunction("setHingeJoint", &Joint3D::setHingeJoint)
        .addFunction("setConeJoint", &Joint3D::setConeJoint)
        .addFunction("setPrismaticJoint", &Joint3D::setPrismaticJoint)
        .addFunction("setSwingTwistJoint", &Joint3D::setSwingTwistJoint)
        .addFunction("setSixDOFJoint", &Joint3D::setSixDOFJoint)
        .addFunction("setPathJoint", &Joint3D::setPathJoint)
        .addFunction("setGearJoint", &Joint3D::setGearJoint)
        .addFunction("setRackAndPinionJoint", &Joint3D::setRackAndPinionJoint)
        .addFunction("setPulleyJoint", &Joint3D::setPulleyJoint)
        .addFunction("getType", &Joint3D::getType)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Contact3D>("Contact3D")
        .addFunction("getBaseOffset", &Contact3D::getBaseOffset)
        .addFunction("getWorldSpaceNormal", &Contact3D::getWorldSpaceNormal)
        .addFunction("getPenetrationDepth", &Contact3D::getPenetrationDepth)
        .addFunction("getShapeIndex1", &Contact3D::getShapeIndex1)
        .addFunction("getShapeIndex2", &Contact3D::getShapeIndex2)
        .addFunction("getRelativeContactPointsOnA", &Contact3D::getRelativeContactPointsOnA)
        .addFunction("getRelativeContactPointsOnB", &Contact3D::getRelativeContactPointsOnB)
        .addProperty("combinedFriction", &Contact3D::getCombinedFriction, &Contact3D::setCombinedFriction)
        .addProperty("combinedRestitution", &Contact3D::getCombinedRestitution, &Contact3D::setCombinedRestitution)
        .addProperty("sensor", &Contact3D::isSensor, &Contact3D::setIsSensor)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<CollideShapeResult3D>("CollideShapeResult3D")
        .addFunction("getContactPointOnA", &CollideShapeResult3D::getContactPointOnA)
        .addFunction("getContactPointOnB", &CollideShapeResult3D::getContactPointOnB)
        .addFunction("getPenetrationAxis", &CollideShapeResult3D::getPenetrationAxis)
        .addFunction("getShapeIndex1", &CollideShapeResult3D::getShapeIndex1)
        .addFunction("getShapeIndex2", &CollideShapeResult3D::getShapeIndex2)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}