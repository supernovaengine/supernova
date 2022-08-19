//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "Object.h"
#include "Camera.h"
#include "Polygon.h"
#include "Terrain.h"
#include "Light.h"
#include "Mesh.h"
#include "Text.h"

using namespace Supernova;

void LuaBinding::registerObjectClasses(lua_State *L){
    sol::state_view lua(L);

    lua.new_usertype<Object>("Object",
	    sol::constructors<Object(Scene*)>(),
        "createChild", &Object::createChild,
        "addChild", &Object::addChild,
        "moveToFirst", &Object::moveToFirst,
        "moveUp", &Object::moveUp,
        "moveDown", &Object::moveDown,
        "moveToLast", &Object::moveToLast,
        "name", sol::property(&Object::getName, &Object::setName),
        "position", sol::property(&Object::getPosition, sol::resolve<void(Vector3)>(&Object::setPosition)),
        "setPosition", sol::overload( sol::resolve<void(float, float, float)>(&Object::setPosition), sol::resolve<void(Vector3)>(&Object::setPosition) ),
        "worldPosition", sol::property(&Object::getWorldPosition),
        "scale", sol::property(&Object::getScale, sol::resolve<void(Vector3)>(&Object::setScale)),
        "setScale", sol::overload( sol::resolve<void(float)>(&Object::setScale), sol::resolve<void(Vector3)>(&Object::setScale) ),
        "worldScale", sol::property(&Object::getWorldScale),
        "mModelMatrix", sol::property(&Object::setModelMatrix),
        "setModelMatrix", &Object::setModelMatrix,
        //"addComponent", &Object::addComponent,
        //"removeComponent", &Object::removeComponent, 
        //"getComponent", &Object::getComponent,
        "entity", sol::property(&Object::getEntity),
        "updateTransform", &Object::updateTransform
        );

    lua.new_usertype<Camera>("Camera",
        sol::constructors<Camera(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "activate", &Camera::activate,
        "setOrtho", &Camera::setOrtho,
        "setPerspective", &Camera::setPerspective,
        "setType", &Camera::setType,
        "type", sol::property(&Camera::getType, &Camera::setType),
        "setView", sol::overload( sol::resolve<void(const float, const float, const float)>(&Camera::setView), sol::resolve<void(Vector3)>(&Camera::setView) ),
        "view", sol::property(&Camera::getView, sol::resolve<void(Vector3)>(&Camera::setView)),
        "setUp", sol::overload( sol::resolve<void(const float, const float, const float)>(&Camera::setUp), sol::resolve<void(Vector3)>(&Camera::setUp) ),
        "up", sol::property(&Camera::getUp, sol::resolve<void(Vector3)>(&Camera::setUp)),
        "rotateView", &Camera::rotateView,
        "rotatePosition", &Camera::rotatePosition,
        "elevateView", &Camera::elevateView,
        "elevatePosition", &Camera::elevatePosition,
        "moveForward", &Camera::moveForward,
        "walkForward", &Camera::walkForward,
        "slide", &Camera::slide,
        "updateCamera", &Camera::updateCamera
        );

    lua.new_usertype<Light>("Light",
        sol::constructors<Light(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "type", sol::property(&Light::getType, &Light::setType),
        "setType", &Light::setType,
        "direction", sol::property(&Light::getDirection, sol::resolve<void(Vector3)>(&Light::setDirection)),
        "setDirection", sol::overload( sol::resolve<void(const float, const float, const float)>(&Light::setDirection), sol::resolve<void(Vector3)>(&Light::setDirection) ),
        "color", sol::property(&Light::getColor, sol::resolve<void(Vector3)>(&Light::setColor)),
        "setColor", sol::overload( sol::resolve<void(Vector3)>(&Light::setColor), sol::resolve<void(const float, const float, const float)>(&Light::setColor) ),
        "range", sol::property(&Light::getRange, &Light::setRange),
        "setRange", &Light::setRange,
        "intensity", sol::property(&Light::getIntensity, &Light::setIntensity),
        "setIntensity", &Light::setIntensity,
        "setConeAngle", &Light::setConeAngle,
        "innerConeAngle", sol::property(&Light::getInnerConeAngle, &Light::setInnerConeAngle),
        "setInnerConeAngle", &Light::setInnerConeAngle,
        "outerConeAngle", sol::property(&Light::getOuterConeAngle, &Light::setOuterConeAngle),
        "setOuterConeAngle", &Light::setOuterConeAngle,
        "shadows", sol::property(&Light::isShadows, &Light::setShadows),
        "setShadows", &Light::setShadows,
        "setShadowCameraNearFar", &Light::setShadowCameraNearFar,
        "cameraNear", sol::property(&Light::getCameraNear, &Light::setCameraNear),
        "setCameraNear", &Light::setCameraNear,
        "cameraFar", sol::property(&Light::getCameraFar, &Light::setCameraFar),
        "setCameraFar", &Light::setCameraFar,
        "numCascades", sol::property(&Light::getNumCascades, &Light::setNumCascades),
        "setNumCascades", &Light::setNumCascades
        );

    lua.new_usertype<Mesh>("Mesh",
        sol::constructors<Mesh(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "texture", sol::property(sol::resolve<void(std::string)>(&Mesh::setTexture)),
        "setTexture", sol::overload( sol::resolve<void(std::string)>(&Mesh::setTexture), sol::resolve<void(FramebufferRender*)>(&Mesh::setTexture) )
        );

    lua.new_usertype<Polygon>("Polygon",
        sol::constructors<Polygon(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "addVertex", sol::overload( sol::resolve<void(float, float)>(&Polygon::addVertex), sol::resolve<void(Vector3)>(&Polygon::addVertex) ),
        "color", sol::property(&Polygon::getColor, sol::resolve<void(Vector4)>(&Polygon::setColor)),
        "setColor", sol::overload( sol::resolve<void(float, float, float, float)>(&Polygon::setColor), sol::resolve<void(Vector4)>(&Polygon::setColor) ),
        "texture", sol::property(sol::resolve<void(std::string)>(&Polygon::setTexture)),
        "setTexture", sol::overload( sol::resolve<void(std::string)>(&Polygon::setTexture), sol::resolve<void(FramebufferRender*)>(&Polygon::setTexture) ),
        "getWidth", &Polygon::getWidth
        );


    lua.new_usertype<Terrain>("Terrain",
        sol::constructors<Terrain(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "heightMap", sol::property(sol::resolve<void(std::string)>(&Terrain::setHeightMap)),
        "setHeightMap", sol::overload( sol::resolve<void(std::string)>(&Terrain::setHeightMap), sol::resolve<void(FramebufferRender*)>(&Terrain::setHeightMap) ),
        "blendMap", sol::property(sol::resolve<void(std::string)>(&Terrain::setBlendMap)),
        "setBlendMap", sol::overload( sol::resolve<void(std::string)>(&Terrain::setBlendMap), sol::resolve<void(FramebufferRender*)>(&Terrain::setBlendMap) ),
        "textureDetailRed", sol::property(&Terrain::setTextureDetailRed),
        "setTextureDetailRed", &Terrain::setTextureDetailRed,
        "textureDetailGreen", sol::property(&Terrain::setTextureDetailGreen),
        "setTextureDetailGreen", &Terrain::setTextureDetailGreen,
        "textureDetailBlue", sol::property(&Terrain::setTextureDetailBlue),
        "setTextureDetailBlue", &Terrain::setTextureDetailBlue,
        "texture", sol::property(sol::resolve<void(std::string)>(&Terrain::setTexture)),
        "setTexture", sol::overload( sol::resolve<void(std::string)>(&Terrain::setTexture), sol::resolve<void(FramebufferRender*)>(&Terrain::setTexture) ),
        "color", sol::property(&Terrain::getColor, sol::resolve<void(Vector4)>(&Terrain::setColor)),
        "setColor", sol::overload( sol::resolve<void(Vector4)>(&Terrain::setColor), sol::resolve<void(float, float, float, float)>(&Terrain::setColor) )  
        );

    lua.new_usertype<Text>("Text",
        sol::constructors<Text(Scene*)>(),
        sol::base_classes, sol::bases<Object>(),
        "setSize", &Text::setSize,
        "width", sol::property(&Text::getWidth, &Text::setWidth),
        "setWidth", &Text::setWidth,
        "height", sol::property(&Text::getHeight, &Text::setHeight),
        "setHeight", &Text::setHeight,
        "maxTextSize", sol::property(&Text::getMaxTextSize, &Text::setMaxTextSize),
        "setMaxTextSize", &Text::setMaxTextSize,
        "text", sol::property(&Text::getText, &Text::setText),
        "setText", &Text::setText,
        "font", sol::property(&Text::getFont, &Text::setFont),
        "setFont", &Text::setFont,
        "fontSize", sol::property(&Text::getFontSize, &Text::setFontSize),
        "setFontSize", &Text::setFontSize,
        "multiline", sol::property(&Text::getMultiline, &Text::setMultiline),
        "setMultiline", &Text::setMultiline,
        "color", sol::property(&Text::getColor, sol::resolve<void(Vector4)>(&Text::setColor)),
        "setColor", sol::overload( sol::resolve<void(Vector4)>(&Text::setColor), sol::resolve<void(float, float, float, float)>(&Text::setColor) ),
        "ascent", sol::property(&Text::getAscent),
        "getAscent", &Text::getAscent,
        "descent", sol::property(&Text::getDescent),
        "getDescent", &Text::getDescent,
        "lineGap", sol::property(&Text::getLineGap),
        "getLineGap", &Text::getLineGap,
        "lineHeight", sol::property(&Text::getLineHeight),
        "getLineHeight", &Text::getLineHeight,
        "numChars", sol::property(&Text::getNumChars),
        "getNumChars", &Text::getNumChars,
        "charPosition", sol::property(&Text::getCharPosition),
        "getCharPosition", &Text::getCharPosition
        );
}