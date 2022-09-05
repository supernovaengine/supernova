//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

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

using namespace Supernova;

void LuaBinding::registerObjectClasses(lua_State *L){
    sol::state_view lua(L);


    lua.new_enum("FogType",
                "LINEAR", FogType::LINEAR,
                "EXPONENTIAL", FogType::EXPONENTIAL,
                "EXPONENTIALSQUARED", FogType::EXPONENTIALSQUARED
                );

    auto fog = lua.new_usertype<Fog>("Fog",
	        sol::default_constructor);

    fog["type"] = sol::property(&Fog::getType, &Fog::setType);
    fog["color"] = sol::property(&Fog::getColor, &Fog::setColor);
    fog["density"] = sol::property(&Fog::getDensity, &Fog::setDensity);
    fog["linearStart"] = sol::property(&Fog::getLinearStart, &Fog::setLinearStart);
    fog["linearEnd"] = sol::property(&Fog::getLinearEnd, &Fog::setLinearEnd);
    fog["setLinearStartEnd"] = &Fog::setLinearStartEnd;


    auto object = lua.new_usertype<Object>("Object",
	    sol::constructors<Object(Scene*)>());

    object["createChild"] = &Object::createChild;
    object["addChild"] = &Object::addChild;
    object["moveToFirst"] = &Object::moveToFirst;
    object["moveUp"] = &Object::moveUp;
    object["moveDown"] = &Object::moveDown;
    object["moveToLast"] = &Object::moveToLast;
    object["name"] = sol::property(&Object::getName, &Object::setName);
    object["position"] = sol::property(&Object::getPosition, sol::resolve<void(Vector3)>(&Object::setPosition));
    object["setPosition"] = sol::overload( sol::resolve<void(float, float, float)>(&Object::setPosition), sol::resolve<void(Vector3)>(&Object::setPosition) );
    object["worldPosition"] = sol::property(&Object::getWorldPosition);
    object["scale"] = sol::property(&Object::getScale, sol::resolve<void(Vector3)>(&Object::setScale));
    object["setScale"] = sol::overload( sol::resolve<void(float)>(&Object::setScale), sol::resolve<void(Vector3)>(&Object::setScale) );
    object["worldScale"] = sol::property(&Object::getWorldScale);
    object["mModelMatrix"] = sol::property(&Object::setModelMatrix);
    object["setModelMatrix"] = &Object::setModelMatrix;
    object["entity"] = sol::property(&Object::getEntity);
    object["updateTransform"] = &Object::updateTransform;

    auto camera = lua.new_usertype<Camera>("Camera",
        sol::constructors<Camera(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    camera["activate"] = &Camera::activate;
    camera["setOrtho"] = &Camera::setOrtho;
    camera["setPerspective"] = &Camera::setPerspective;
    camera["setType"] = &Camera::setType;
    camera["type"] = sol::property(&Camera::getType, &Camera::setType);
    camera["setView"] = sol::overload( sol::resolve<void(const float, const float, const float)>(&Camera::setView), sol::resolve<void(Vector3)>(&Camera::setView) );
    camera["view"] = sol::property(&Camera::getView, sol::resolve<void(Vector3)>(&Camera::setView));
    camera["setUp"] = sol::overload( sol::resolve<void(const float, const float, const float)>(&Camera::setUp), sol::resolve<void(Vector3)>(&Camera::setUp) );
    camera["up"] = sol::property(&Camera::getUp, sol::resolve<void(Vector3)>(&Camera::setUp));
    camera["rotateView"] = &Camera::rotateView;
    camera["rotatePosition"] = &Camera::rotatePosition;
    camera["elevateView"] = &Camera::elevateView;
    camera["elevatePosition"] = &Camera::elevatePosition;
    camera["moveForward"] = &Camera::moveForward;
    camera["walkForward"] = &Camera::walkForward;
    camera["slide"] = &Camera::slide;
    camera["updateCamera"] = &Camera::updateCamera;

    auto light = lua.new_usertype<Light>("Light",
        sol::constructors<Light(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    light["type"] = sol::property(&Light::getType, &Light::setType);
    light["setType"] = &Light::setType;
    light["direction"] = sol::property(&Light::getDirection, sol::resolve<void(Vector3)>(&Light::setDirection));
    light["setDirection"] = sol::overload( sol::resolve<void(const float, const float, const float)>(&Light::setDirection), sol::resolve<void(Vector3)>(&Light::setDirection) );
    light["color"] = sol::property(&Light::getColor, sol::resolve<void(Vector3)>(&Light::setColor));
    light["setColor"] = sol::overload( sol::resolve<void(Vector3)>(&Light::setColor), sol::resolve<void(const float, const float, const float)>(&Light::setColor) );
    light["range"] = sol::property(&Light::getRange, &Light::setRange);
    light["setRange"] = &Light::setRange;
    light["intensity"] = sol::property(&Light::getIntensity, &Light::setIntensity);
    light["setIntensity"] = &Light::setIntensity;
    light["setConeAngle"] = &Light::setConeAngle;
    light["innerConeAngle"] = sol::property(&Light::getInnerConeAngle, &Light::setInnerConeAngle);
    light["setInnerConeAngle"] = &Light::setInnerConeAngle;
    light["outerConeAngle"] = sol::property(&Light::getOuterConeAngle, &Light::setOuterConeAngle);
    light["setOuterConeAngle"] = &Light::setOuterConeAngle;
    light["shadows"] = sol::property(&Light::isShadows, &Light::setShadows);
    light["setShadows"] = &Light::setShadows;
    light["setShadowCameraNearFar"] = &Light::setShadowCameraNearFar;
    light["cameraNear"] = sol::property(&Light::getCameraNear, &Light::setCameraNear);
    light["setCameraNear"] = &Light::setCameraNear;
    light["cameraFar"] = sol::property(&Light::getCameraFar, &Light::setCameraFar);
    light["setCameraFar"] = &Light::setCameraFar;
    light["numCascades"] = sol::property(&Light::getNumCascades, &Light::setNumCascades);
    light["setNumCascades"] = &Light::setNumCascades;

    auto mesh = lua.new_usertype<Mesh>("Mesh",
        sol::constructors<Mesh(Scene*)>(),
        sol::base_classes, sol::bases<Object>());
    
    mesh["texture"] = sol::property(sol::resolve<void(std::string)>(&Mesh::setTexture));
    mesh["setTexture"] = sol::overload( sol::resolve<void(std::string)>(&Mesh::setTexture), sol::resolve<void(FramebufferRender*)>(&Mesh::setTexture) );

    auto polygon = lua.new_usertype<Polygon>("Polygon",
        sol::constructors<Polygon(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    polygon["addVertex"] = sol::overload( sol::resolve<void(float, float)>(&Polygon::addVertex), sol::resolve<void(Vector3)>(&Polygon::addVertex) );
    polygon["color"] = sol::property(&Polygon::getColor, sol::resolve<void(Vector4)>(&Polygon::setColor));
    polygon["setColor"] = sol::overload( sol::resolve<void(float, float, float, float)>(&Polygon::setColor), sol::resolve<void(Vector4)>(&Polygon::setColor) );
    polygon["texture"] = sol::property(sol::resolve<void(std::string)>(&Polygon::setTexture));
    polygon["setTexture"] = sol::overload( sol::resolve<void(std::string)>(&Polygon::setTexture), sol::resolve<void(FramebufferRender*)>(&Polygon::setTexture) );
    polygon["getWidth"] = &Polygon::getWidth;


    auto terrain = lua.new_usertype<Terrain>("Terrain",
        sol::constructors<Terrain(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    terrain["heightMap"] = sol::property(sol::resolve<void(std::string)>(&Terrain::setHeightMap));
    terrain["setHeightMap"] = sol::overload( sol::resolve<void(std::string)>(&Terrain::setHeightMap), sol::resolve<void(FramebufferRender*)>(&Terrain::setHeightMap) );
    terrain["blendMap"] = sol::property(sol::resolve<void(std::string)>(&Terrain::setBlendMap));
    terrain["setBlendMap"] = sol::overload( sol::resolve<void(std::string)>(&Terrain::setBlendMap), sol::resolve<void(FramebufferRender*)>(&Terrain::setBlendMap) );
    terrain["textureDetailRed"] = sol::property(&Terrain::setTextureDetailRed);
    terrain["setTextureDetailRed"] = &Terrain::setTextureDetailRed;
    terrain["textureDetailGreen"] = sol::property(&Terrain::setTextureDetailGreen);
    terrain["setTextureDetailGreen"] = &Terrain::setTextureDetailGreen;
    terrain["textureDetailBlue"] = sol::property(&Terrain::setTextureDetailBlue);
    terrain["setTextureDetailBlue"] = &Terrain::setTextureDetailBlue;
    terrain["texture"] = sol::property(sol::resolve<void(std::string)>(&Terrain::setTexture));
    terrain["setTexture"] = sol::overload( sol::resolve<void(std::string)>(&Terrain::setTexture), sol::resolve<void(FramebufferRender*)>(&Terrain::setTexture) );
    terrain["color"] = sol::property(&Terrain::getColor, sol::resolve<void(Vector4)>(&Terrain::setColor));
    terrain["setColor"] = sol::overload( sol::resolve<void(Vector4)>(&Terrain::setColor), sol::resolve<void(float, float, float, float)>(&Terrain::setColor) );

    lua.new_usertype<Bone>("Bone",
        sol::constructors<Bone(Scene*, Entity)>(),
        sol::base_classes, sol::bases<Object>()
        );

    auto model = lua.new_usertype<Model>("Model",
        sol::constructors<Model(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    model["loadOBJ"] = &Model::loadOBJ;
    model["loadGLTF"] = &Model::loadGLTF;
    model["loadModel"] = &Model::loadModel;
    model["getAnimation"] = &Model::getAnimation;
    model["findAnimation"] = &Model::findAnimation;
    model["getBone"] = sol::overload( sol::resolve<Bone(std::string)>(&Model::getBone), sol::resolve<Bone(int)>(&Model::getBone) );
    model["getMorphWeight"] = sol::overload( sol::resolve<float(std::string)>(&Model::getMorphWeight), sol::resolve<float(int)>(&Model::getMorphWeight) );
    model["setMorphWeight"] = sol::overload( sol::resolve<void(std::string, float)>(&Model::setMorphWeight), sol::resolve<void(int, float)>(&Model::setMorphWeight) );


    auto meshpolygon = lua.new_usertype<MeshPolygon>("MeshPolygon",
        sol::constructors<MeshPolygon(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    meshpolygon["addVertex"] = sol::overload( sol::resolve<void(Vector3)>(&MeshPolygon::addVertex), sol::resolve<void(float, float)>(&MeshPolygon::addVertex) );
    meshpolygon["width"] = sol::property(&MeshPolygon::getWidth);
    meshpolygon["getWidth"] = &MeshPolygon::getWidth;
    meshpolygon["height"] = sol::property(&MeshPolygon::getHeight);
    meshpolygon["getHeight"] = &MeshPolygon::getHeight;
    meshpolygon["flipY"] = sol::property(&MeshPolygon::isFlipY, &MeshPolygon::setFlipY);
    meshpolygon["setFlipY"] = &MeshPolygon::setFlipY;
    meshpolygon["isFlipY"] = &MeshPolygon::isFlipY;

    auto particles = lua.new_usertype<Particles>("Particles",
        sol::constructors<Particles(Scene*)>(),
        sol::base_classes, sol::bases<Object>());
        
    particles["maxParticles"]  = sol::property(&Particles::getMaxParticles, &Particles::setMaxParticles);
    particles["setMaxParticles"] = &Particles::setMaxParticles;
    particles["getMaxParticles"] = &Particles::getMaxParticles;
    particles["addParticle"] = sol::overload( 
        sol::resolve<void(Vector3)>(&Particles::addParticle), 
        sol::resolve<void(Vector3, Vector4)>(&Particles::addParticle),
        sol::resolve<void(Vector3, Vector4, float, float)>(&Particles::addParticle),
        sol::resolve<void(Vector3, Vector4, float, float, Rect)>(&Particles::addParticle),
        sol::resolve<void(float, float, float)>(&Particles::addParticle) );
    particles["addSpriteFrame"] = sol::overload( 
        sol::resolve<void(int, std::string, Rect)>(&Particles::addSpriteFrame), 
        sol::resolve<void(std::string, float, float, float, float)>(&Particles::addSpriteFrame),
        sol::resolve<void(float, float, float, float)>(&Particles::addSpriteFrame),
        sol::resolve<void(Rect)>(&Particles::addSpriteFrame));
    particles["removeSpriteFrame"] = sol::overload( 
        sol::resolve<void(int)>(&Particles::removeSpriteFrame), 
        sol::resolve<void(std::string)>(&Particles::removeSpriteFrame));
    particles["setTexture"] = &Particles::setTexture;

    auto planeterrain = lua.new_usertype<PlaneTerrain>("PlaneTerrain",
        sol::constructors<PlaneTerrain(Scene*)>(),
        sol::base_classes, sol::bases<Object>());
    
    planeterrain["create"] = &PlaneTerrain::create;

    auto sprite = lua.new_usertype<Sprite>("Sprite",
        sol::constructors<Sprite(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    sprite["setSize"] = &Sprite::setSize;
    sprite["width"] = sol::property(&Sprite::getWidth,  &Sprite::setWidth);
    sprite["setWidth"] = &Sprite::setWidth;
    sprite["getWidth"] = &Sprite::getWidth;
    sprite["height"] = sol::property(&Sprite::getHeight,  &Sprite::setHeight);
    sprite["setHeight"] = &Sprite::setHeight;
    sprite["getHeight"] = &Sprite::getHeight;
    sprite["flipY"] = sol::property(&Sprite::isFlipY, &Sprite::setFlipY);
    sprite["setFlipY"] = &Sprite::setFlipY;
    sprite["isFlipY"] = &Sprite::isFlipY;
    sprite["setBillboard"] = &Sprite::setBillboard;
    sprite["textureRect"] = sol::property(&Sprite::getTextureRect, sol::resolve<void(Rect)>(&Sprite::setTextureRect));
    sprite["setTextureRect"] = sol::overload( 
        sol::resolve<void(Rect)>(&Sprite::setTextureRect), 
        sol::resolve<void(float, float, float, float)>(&Sprite::setTextureRect) );
    sprite["getTextureRect"] = &Sprite::getTextureRect;
    sprite["addFrame"] = sol::overload( 
        sol::resolve<void(int, std::string, Rect)>(&Sprite::addFrame), 
        sol::resolve<void(std::string, float, float, float, float)>(&Sprite::addFrame),
        sol::resolve<void(float, float, float, float)>(&Sprite::addFrame),
        sol::resolve<void(Rect)>(&Sprite::addFrame) );
    sprite["removeFrame"] = sol::overload( 
        sol::resolve<void(int)>(&Sprite::removeFrame), 
        sol::resolve<void(std::string)>(&Sprite::removeFrame) );
    sprite["setFrame"] = sol::overload( 
        sol::resolve<void(int)>(&Sprite::setFrame), 
        sol::resolve<void(std::string)>(&Sprite::setFrame) );
    sprite["startAnimation"] = sol::overload( 
        sol::resolve<void(std::vector<int>, std::vector<int>, bool)>(&Sprite::startAnimation), 
        sol::resolve<void(int, int, int, bool)>(&Sprite::startAnimation) );
    sprite["pauseAnimation"] = &Sprite::pauseAnimation;
    sprite["stopAnimation"] = &Sprite::stopAnimation;

    auto text = lua.new_usertype<Text>("Text",
        sol::constructors<Text(Scene*)>(),
        sol::base_classes, sol::bases<Object>());

    text["setSize"] = &Text::setSize;
    text["width"] = sol::property(&Text::getWidth, &Text::setWidth);
    text["setWidth"] = &Text::setWidth;
    text["height"] = sol::property(&Text::getHeight, &Text::setHeight);
    text["setHeight"] = &Text::setHeight;
    text["maxTextSize"] = sol::property(&Text::getMaxTextSize, &Text::setMaxTextSize);
    text["setMaxTextSize"] = &Text::setMaxTextSize;
    text["text"] = sol::property(&Text::getText, &Text::setText);
    text["setText"] = &Text::setText;
    text["font"] = sol::property(&Text::getFont, &Text::setFont);
    text["setFont"] = &Text::setFont;
    text["fontSize"] = sol::property(&Text::getFontSize, &Text::setFontSize);
    text["setFontSize"] = &Text::setFontSize;
    text["multiline"] = sol::property(&Text::getMultiline, &Text::setMultiline);
    text["setMultiline"] = &Text::setMultiline;
    text["color"] = sol::property(&Text::getColor, sol::resolve<void(Vector4)>(&Text::setColor));
    text["setColor"] = sol::overload( sol::resolve<void(Vector4)>(&Text::setColor), sol::resolve<void(float, float, float, float)>(&Text::setColor) );
    text["ascent"] = sol::property(&Text::getAscent);
    text["getAscent"] = &Text::getAscent;
    text["descent"] = sol::property(&Text::getDescent);
    text["getDescent"] = &Text::getDescent;
    text["lineGap"] = sol::property(&Text::getLineGap);
    text["getLineGap"] = &Text::getLineGap;
    text["lineHeight"] = sol::property(&Text::getLineHeight);
    text["getLineHeight"] = &Text::getLineHeight;
    text["numChars"] = sol::property(&Text::getNumChars);
    text["getNumChars"] = &Text::getNumChars;
    text["charPosition"] = sol::property(&Text::getCharPosition);
    text["getCharPosition"] = &Text::getCharPosition;

}