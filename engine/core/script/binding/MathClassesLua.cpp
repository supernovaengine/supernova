//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"
#include "LuaBridgeAddon.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Rect.h"
#include "Ray.h"
#include "Plane.h"
#include "AlignedBox.h"

using namespace Supernova;

namespace luabridge
{
    template<> struct Stack<Plane::Side> : EnumWrapper<Plane::Side>{};
    template<> struct Stack<AlignedBox::BoxType> : EnumWrapper<AlignedBox::BoxType>{};
    template<> struct Stack<AlignedBox::CornerEnum> : EnumWrapper<AlignedBox::CornerEnum>{};
}

void LuaBinding::registerMathClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginClass<Vector2>("Vector2")
        .addConstructor<void(), void(float, float)>()
        .addProperty("x", &Vector2::x, true)
        .addProperty("y", &Vector2::y, true)
        .addFunction("__tostring", &Vector2::toString)
        .addFunction("__eq", &Vector2::operator==)
        .addFunction("__lt", &Vector2::operator<)
        .addFunction("__sub", (Vector2 (Vector2::*)(const Vector2&) const)&Vector2::operator-)
        .addFunction("__add", (Vector2 (Vector2::*)(const Vector2&) const)&Vector2::operator+)
        .addFunction("__div", 
            luabridge::overload<float>(&Vector2::operator/),
            luabridge::overload<const Vector2&>(&Vector2::operator/))
        .addFunction("__mul", 
            luabridge::overload<float>(&Vector2::operator*),
            luabridge::overload<const Vector2&>(&Vector2::operator*))
        .addFunction("__unm", (Vector2 (Vector2::*)() const)&Vector2::operator-)
        .addFunction("swap", &Vector2::swap)
        .addFunction("length", &Vector2::length)
        .addFunction("squaredLength", &Vector2::squaredLength)
        .addFunction("distance", &Vector2::distance)
        .addFunction("squaredDistance", &Vector2::squaredDistance)
        .addFunction("dotProduct", &Vector2::dotProduct)
        .addFunction("normalize", &Vector2::normalize)
        .addFunction("midPoint", &Vector2::midPoint)
        .addFunction("makeFloor", &Vector2::makeFloor)
        .addFunction("makeCeil", &Vector2::makeCeil)
        .addFunction("perpendicular", &Vector2::perpendicular)
        .addFunction("crossProduct", &Vector2::crossProduct)
        .addFunction("normalizedCopy", &Vector2::normalizedCopy)
        .addFunction("reflect", &Vector2::reflect)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Vector3>("Vector3")
        .addConstructor<void(), void(float, float, float)>()
        .addProperty("x", &Vector3::x, true)
        .addProperty("y", &Vector3::y, true)
        .addProperty("z", &Vector3::z, true)
        .addFunction("__tostring", &Vector3::toString)
        .addFunction("__eq", &Vector3::operator==)
        .addFunction("__lt", &Vector3::operator<)
        .addFunction("__sub", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator-)
        .addFunction("__add", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator+)
        .addFunction("__div", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator/)
        .addFunction("__mul", 
            luabridge::overload<float>(&Vector3::operator*), // need float operator first to fix estrange error in Emscripten
            luabridge::overload<const Vector3&>(&Vector3::operator*))
        .addFunction("__unm", (Vector3 (Vector3::*)() const)&Vector3::operator-)
        .addFunction("length", &Vector3::length)
        .addFunction("squaredLength", &Vector3::squaredLength)
        .addFunction("dotProduct", &Vector3::dotProduct)
        .addFunction("absDotProduct", &Vector3::absDotProduct)
        .addFunction("distance", &Vector3::distance)
        .addFunction("squaredDistance", &Vector3::squaredDistance)
        .addFunction("normalize", &Vector3::normalize)
        .addFunction("normalizeL", &Vector3::normalizeL)
        .addFunction("crossProduct", &Vector3::crossProduct)
        .addFunction("midPoint", &Vector3::midPoint)
        .addFunction("makeFloor", &Vector3::makeFloor)
        .addFunction("makeCeil", &Vector3::makeCeil)
        .addFunction("perpendicular", &Vector3::perpendicular)
        .addFunction("reflect", &Vector3::reflect)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Vector4>("Vector4")
        .addConstructor<void(), void(float, float, float, float)>()
        .addProperty("x", &Vector4::x, true)
        .addProperty("y", &Vector4::y, true)
        .addProperty("z", &Vector4::z, true)
        .addProperty("w", &Vector4::w, true)
        .addFunction("__tostring", &Vector4::toString)
        .addFunction("__eq", &Vector4::operator==)
        .addFunction("__lt", &Vector4::operator<)
        .addFunction("__sub", (Vector4 (Vector4::*)(const Vector4&) const)&Vector4::operator-)
        .addFunction("__add", (Vector4 (Vector4::*)(const Vector4&) const)&Vector4::operator+)
        .addFunction("__div", 
            luabridge::overload<float>(&Vector4::operator/),
            luabridge::overload<const Vector4&>(&Vector4::operator/))
        .addFunction("__mul", 
            luabridge::overload<float>(&Vector4::operator*),
            luabridge::overload<const Vector4&>(&Vector4::operator*))
        .addFunction("__unm", (Vector4 (Vector4::*)() const)&Vector4::operator-)
        .addFunction("swap", &Vector4::swap)
        .addFunction("divideByW", &Vector4::divideByW)
        .addFunction("dotProduct", &Vector4::dotProduct)
        .addFunction("isNaN", &Vector4::isNaN)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Rect>("Rect")
        .addConstructor<void(), void(float, float, float, float)>()
        .addProperty("x", &Rect::getX, &Rect::setX)
        .addProperty("y", &Rect::getY, &Rect::setY)
        .addProperty("width", &Rect::getWidth, &Rect::setWidth)
        .addProperty("height", &Rect::getHeight, &Rect::setHeight)
        .addFunction("__tostring", &Rect::toString)
        .addFunction("__eq", &Rect::operator==)
        .addFunction("getVector", &Rect::getVector)
        .addFunction("setRect", 
            luabridge::overload<Rect>(&Rect::setRect),
            luabridge::overload<float, float, float, float>(&Rect::setRect))
        .addFunction("fitOnRect", &Rect::fitOnRect)
        .addFunction("isNormalized", &Rect::isNormalized)
        .addFunction("isZero", &Rect::isZero)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Matrix3>("Matrix3")
        .addConstructor<
            void(), 
            void(
                float, float, float, 
                float, float, float, 
                float, float, float)>()
        .addFunction("__tostring", &Matrix3::toString)
        .addFunction("__eq", &Matrix3::operator==)
        .addFunction("__sub", (Matrix3 (Matrix3::*)(const Matrix3&) const)&Matrix3::operator-)
        .addFunction("__add", (Matrix3 (Matrix3::*)(const Matrix3&) const)&Matrix3::operator+)
        .addFunction("__mul", (Matrix3 (Matrix3::*)(const Matrix3&) const)&Matrix3::operator*)
        .addFunction("row", &Matrix3::row)
        .addFunction("column", &Matrix3::column)
        .addFunction("set", &Matrix3::set)
        .addFunction("get", &Matrix3::get)
        .addFunction("setRow", &Matrix3::setRow)
        .addFunction("setColumn", &Matrix3::setColumn)
        .addFunction("identity", &Matrix3::identity)
        .addFunction("calcInverse", &Matrix3::calcInverse)
        .addFunction("inverse", &Matrix3::inverse)
        .addFunction("transpose", &Matrix3::transpose)
        .addStaticFunction("rotateMatrix", 
            luabridge::overload<const float, const Vector3&>(&Matrix3::rotateMatrix),
            luabridge::overload<const float, const float>(&Matrix3::rotateMatrix))
        .addStaticFunction("rotateXMatrix", &Matrix3::rotateXMatrix)
        .addStaticFunction("rotateYMatrix", &Matrix3::rotateYMatrix)
        .addStaticFunction("rotateZMatrix", &Matrix3::rotateZMatrix)
        .addStaticFunction("scaleMatrix", 
            luabridge::overload<const Vector3&>(&Matrix3::scaleMatrix),
            luabridge::overload<const float>(&Matrix3::scaleMatrix))
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Matrix4>("Matrix4")
        .addConstructor<
            void(), 
            void(
                float, float, float, float,
                float, float, float, float,
                float, float, float, float,
                float, float, float, float)>()
        .addFunction("__tostring", &Matrix4::toString)
        .addFunction("__eq", &Matrix4::operator==)
        .addFunction("__sub", (Matrix4 (Matrix4::*)(const Matrix4&) const)&Matrix4::operator-)
        .addFunction("__add", (Matrix4 (Matrix4::*)(const Matrix4&) const)&Matrix4::operator+)
        .addFunction("__mul", (Matrix4 (Matrix4::*)(const Matrix4&) const)&Matrix4::operator*)
        .addFunction("row", &Matrix4::row)
        .addFunction("column", &Matrix4::column)
        .addFunction("set", &Matrix4::set)
        .addFunction("get", &Matrix4::get)
        .addFunction("setRow", &Matrix4::setRow)
        .addFunction("setColumn", &Matrix4::setColumn)
        .addFunction("identity", &Matrix4::identity)
        .addFunction("translateInPlace", &Matrix4::translateInPlace)
        .addFunction("inverse", &Matrix4::inverse)
        .addFunction("transpose", &Matrix4::transpose)
        .addFunction("determinant", &Matrix4::determinant)
        .addStaticFunction("translateMatrix", 
            luabridge::overload<const Vector3&>(&Matrix4::translateMatrix),
            luabridge::overload<const float, const float, const float>(&Matrix4::translateMatrix))
        .addStaticFunction("rotateMatrix", 
            luabridge::overload<const float, const Vector3&>(&Matrix4::rotateMatrix),
            luabridge::overload<const float, const float>(&Matrix4::rotateMatrix))
        .addStaticFunction("rotateXMatrix", &Matrix4::rotateXMatrix)
        .addStaticFunction("rotateYMatrix", &Matrix4::rotateYMatrix)
        .addStaticFunction("rotateZMatrix", &Matrix4::rotateZMatrix)
        .addStaticFunction("scaleMatrix", 
            luabridge::overload<const Vector3&>(&Matrix4::scaleMatrix),
            luabridge::overload<const float>(&Matrix4::scaleMatrix))
        .addStaticFunction("lookAtMatrix", &Matrix4::lookAtMatrix)
        .addStaticFunction("frustumMatrix", &Matrix4::frustumMatrix)
        .addStaticFunction("orthoMatrix", &Matrix4::orthoMatrix)
        .addStaticFunction("perspectiveMatrix", &Matrix4::perspectiveMatrix)
        .addFunction("decompose", &Matrix4::decompose)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Quaternion>("Quaternion")
        .addConstructor<void(), void(float, float, float, float)>()
        .addFunction("__tostring", &Quaternion::toString)
        .addFunction("__eq", &Quaternion::operator==)
        .addFunction("__sub", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator-)
        .addFunction("__add", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator+)
        .addFunction("__mul", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator*)
        .addFunction("__unm", (Quaternion (Quaternion::*)() const)&Quaternion::operator-)
        .addFunction("fromAxes", 
            luabridge::overload<const Vector3*>(&Quaternion::fromAxes),
            luabridge::overload<const Vector3&, const Vector3&, const Vector3&>(&Quaternion::fromAxes))
        .addFunction("fromRotationMatrix", &Quaternion::fromRotationMatrix)
        .addFunction("getRotationMatrix", &Quaternion::getRotationMatrix)
        .addFunction("fromAngle", &Quaternion::fromAngle)
        .addFunction("fromAngleAxis", &Quaternion::fromAngleAxis)
        .addFunction("xAxis", &Quaternion::xAxis)
        .addFunction("yAxis", &Quaternion::yAxis)
        .addFunction("zAxis", &Quaternion::zAxis)
        .addFunction("dot", &Quaternion::dot)
        .addFunction("norm", &Quaternion::norm)
        .addFunction("inverse", &Quaternion::inverse)
        .addFunction("unitInverse", &Quaternion::unitInverse)
        .addFunction("exp", &Quaternion::exp)
        .addFunction("log", &Quaternion::log)
        .addFunction("slerp", &Quaternion::slerp)
        .addFunction("slerpExtraSpins", &Quaternion::slerpExtraSpins)
        .addFunction("squad", &Quaternion::squad)
        .addFunction("nlerp", &Quaternion::nlerp)
        .addFunction("normalise", &Quaternion::normalise)
        .addFunction("getRoll", &Quaternion::getRoll)
        .addFunction("getPitch", &Quaternion::getPitch)
        .addFunction("getYaw", &Quaternion::getYaw)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Plane>("Plane")
        .addConstructor<
            void(), 
            void(const Vector3&, float), 
            void(float, float, float, float), 
            void(const Vector3&, const Vector3&), 
            void(const Vector3&, const Vector3&, const Vector3&)>()
        .addFunction("__unm", (Plane (Plane::*)() const)&Plane::operator-)
        .addFunction("__eq", &Plane::operator==)
        .addFunction("getSide", 
            luabridge::overload<const Vector3&>(&Plane::getSide),
            luabridge::overload<const Vector3&, const Vector3&>(&Plane::getSide),
            luabridge::overload<const AlignedBox&>(&Plane::getSide))
        .addFunction("getDistance", &Plane::getDistance)
        .addFunction("redefine", 
            luabridge::overload<const Vector3&, const Vector3&, const Vector3&>(&Plane::redefine),
            luabridge::overload<const Vector3&, const Vector3&>(&Plane::redefine))
        .addFunction("projectVector", &Plane::projectVector)
        .addFunction("normalize", &Plane::normalize)
        .addStaticProperty("Side", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addVariable("NO_SIDE", Plane::Side::NO_SIDE)
                .addVariable("POSITIVE_SIDE", Plane::Side::POSITIVE_SIDE)
                .addVariable("NEGATIVE_SIDE", Plane::Side::NEGATIVE_SIDE)
                .addVariable("BOTH_SIDE", Plane::Side::BOTH_SIDE);
            table.pop();
            return table;
        })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<AlignedBox>("AlignedBox")
        .addConstructor<
            void(), 
            void(AlignedBox::BoxType), 
            void(const Vector3&, const Vector3&), 
            void(float, float, float, float, float, float)>()
        .addProperty("minimum", (const Vector3&(AlignedBox::*)() const)&AlignedBox::getMinimum, (void(AlignedBox::*)(const Vector3&))&AlignedBox::setMinimum)
        .addFunction("setMinimum", (void(AlignedBox::*)(float, float, float))&AlignedBox::setMinimum)
        .addFunction("setMinimumX", &AlignedBox::setMinimumX)
        .addFunction("setMinimumY", &AlignedBox::setMinimumY)
        .addFunction("setMinimumZ", &AlignedBox::setMinimumZ)
        .addProperty("maximum", (const Vector3&(AlignedBox::*)() const)&AlignedBox::getMaximum, (void(AlignedBox::*)(const Vector3&))&AlignedBox::setMaximum)
        .addFunction("setMaximum", (void(AlignedBox::*)(float, float, float))&AlignedBox::setMaximum)
        .addFunction("setMaximumX", &AlignedBox::setMaximumX)
        .addFunction("setMaximumY", &AlignedBox::setMaximumY)
        .addFunction("setMaximumZ", &AlignedBox::setMaximumZ)
        .addFunction("setExtents", 
            luabridge::overload<const Vector3&, const Vector3&>(&AlignedBox::setExtents),
            luabridge::overload<float, float, float, float, float, float>(&AlignedBox::setExtents))
        .addFunction("getAllCorners", &AlignedBox::getAllCorners)
        .addFunction("getCorner", &AlignedBox::getCorner)
        .addFunction("merge", 
            luabridge::overload<const AlignedBox&>(&AlignedBox::merge),
            luabridge::overload<const Vector3&>(&AlignedBox::merge))
        .addFunction("transform", &AlignedBox::transform)
        .addFunction("isNull", &AlignedBox::isNull)
        .addFunction("setNull", &AlignedBox::setNull)
        .addFunction("isFinite", &AlignedBox::isFinite)
        .addFunction("isInfinite", &AlignedBox::isInfinite)
        .addFunction("setInfinite", &AlignedBox::isNull)
        .addFunction("intersects", 
            luabridge::overload<const AlignedBox&>(&AlignedBox::intersects),
            luabridge::overload<const Plane&>(&AlignedBox::intersects),
            luabridge::overload<const Vector3&>(&AlignedBox::intersects))
        .addFunction("intersection", &AlignedBox::intersection)
        .addFunction("volume", &AlignedBox::volume)
        .addFunction("scale", &AlignedBox::scale)
        .addFunction("getCenter", &AlignedBox::getCenter)
        .addFunction("getSize", &AlignedBox::getSize)
        .addFunction("getHalfSize", &AlignedBox::getHalfSize)
        .addFunction("contains", 
            luabridge::overload<const AlignedBox&>(&AlignedBox::contains),
            luabridge::overload<const Vector3&>(&AlignedBox::contains))
        .addFunction("squaredDistance", &AlignedBox::squaredDistance)
        .addFunction("distance", &AlignedBox::distance)
        .addStaticProperty("BoxType", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addVariable("BOXTYPE_NULL", AlignedBox::BoxType::BOXTYPE_NULL)
                .addVariable("BOXTYPE_FINITE", AlignedBox::BoxType::BOXTYPE_FINITE)
                .addVariable("BOXTYPE_INFINITE", AlignedBox::BoxType::BOXTYPE_INFINITE);
            table.pop();
            return table;
            })
        .addStaticProperty("CornerEnum", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addVariable("FAR_LEFT_BOTTOM", AlignedBox::CornerEnum::FAR_LEFT_BOTTOM)
                .addVariable("FAR_LEFT_TOP", AlignedBox::CornerEnum::FAR_LEFT_TOP)
                .addVariable("FAR_RIGHT_TOP", AlignedBox::CornerEnum::FAR_RIGHT_TOP)
                .addVariable("FAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::FAR_RIGHT_BOTTOM)
                .addVariable("NEAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM)
                .addVariable("NEAR_LEFT_BOTTOM", AlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)
                .addVariable("NEAR_LEFT_TOP", AlignedBox::CornerEnum::NEAR_LEFT_TOP)
                .addVariable("NEAR_RIGHT_TOP", AlignedBox::CornerEnum::NEAR_RIGHT_TOP);
            table.pop();
            return table;
            })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Ray>("Ray")
        .addConstructor<void(), void(Vector3, Vector3)>()
        .addProperty("origin", &Ray::getOrigin, &Ray::setOrigin)
        .addProperty("direction", &Ray::getDirection, &Ray::setDirection)
        .addFunction("getPoint", &Ray::getPoint)
        .addFunction("intersects", 
            luabridge::overload<AlignedBox>(&Ray::intersects),
            luabridge::overload<Plane>(&Ray::intersects))
        .addFunction("intersectionPoint", 
            luabridge::overload<AlignedBox>(&Ray::intersectionPoint),
            luabridge::overload<Plane>(&Ray::intersectionPoint))
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}