//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"
#include "EnumWrapper.h"

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
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Vector2();
                if (lua_gettop(L) == 4)
                    return new (ptr) Vector2(luaL_checknumber(L, 2), luaL_checknumber(L, 3)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addProperty("x", &Vector2::x, true)
        .addProperty("y", &Vector2::y, true)
        .addFunction("__tostring", &Vector2::toString)
        .addFunction("__eq", &Vector2::operator==)
        .addFunction("__lt", &Vector2::operator<)
        .addFunction("__sub", (Vector2 (Vector2::*)(const Vector2&) const)&Vector2::operator-)
        .addFunction("__add", (Vector2 (Vector2::*)(const Vector2&) const)&Vector2::operator+)
        .addFunction("__div", +[](Vector2* self, lua_State* L) -> Vector2 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, -1)) return self->operator/(lua_tonumber(L, -1));
            if (luabridge::Stack<Vector2>::isInstance(L, -1)) return self->operator/(luabridge::Stack<Vector2>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("__mul", +[](Vector2* self, lua_State* L) -> Vector2 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, -1)) return self->operator*(lua_tonumber(L, -1));
            if (luabridge::Stack<Vector2>::isInstance(L, -1)) return self->operator*(luabridge::Stack<Vector2>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
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
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Vector3();
                if (lua_gettop(L) == 5)
                    return new (ptr) Vector3(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addProperty("x", &Vector3::x, true)
        .addProperty("y", &Vector3::y, true)
        .addProperty("z", &Vector3::z, true)
        .addFunction("__tostring", &Vector3::toString)
        .addFunction("__eq", &Vector3::operator==)
        .addFunction("__lt", &Vector3::operator<)
        .addFunction("__sub", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator-)
        .addFunction("__add", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator+)
        .addFunction("__div", (Vector3 (Vector3::*)(const Vector3&) const)&Vector3::operator/)
        .addFunction("__mul", +[](Vector3* self, lua_State* L) -> Vector3 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, -1)) return self->operator*(lua_tonumber(L, -1));
            if (luabridge::Stack<Vector3>::isInstance(L, -1)) return self->operator*(luabridge::Stack<Vector3>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
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
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Vector4>("Vector4")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Vector4();
                if (lua_gettop(L) == 6)
                    return new (ptr) Vector4(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addConstructor <void (*) (float, float, float, float)> ()
        .addProperty("x", &Vector4::x, true)
        .addProperty("y", &Vector4::y, true)
        .addProperty("z", &Vector4::z, true)
        .addProperty("w", &Vector4::w, true)
        .addFunction("__tostring", &Vector4::toString)
        .addFunction("__eq", &Vector4::operator==)
        .addFunction("__lt", &Vector4::operator<)
        .addFunction("__sub", (Vector4 (Vector4::*)(const Vector4&) const)&Vector4::operator-)
        .addFunction("__add", (Vector4 (Vector4::*)(const Vector4&) const)&Vector4::operator+)
        .addFunction("__div", +[](Vector4* self, lua_State* L) -> Vector4 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, -1)) return self->operator/(lua_tonumber(L, -1));
            if (luabridge::Stack<Vector4>::isInstance(L, -1)) return self->operator/(luabridge::Stack<Vector4>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("__mul", +[](Vector4* self, lua_State* L) -> Vector4 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isnumber(L, -1)) return self->operator*(lua_tonumber(L, -1));
            if (luabridge::Stack<Vector4>::isInstance(L, -1)) return self->operator*(luabridge::Stack<Vector4>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("__unm", (Vector4 (Vector4::*)() const)&Vector4::operator-)
        .addFunction("swap", &Vector4::swap)
        .addFunction("divideByW", &Vector4::divideByW)
        .addFunction("dotProduct", &Vector4::dotProduct)
        .addFunction("isNaN", &Vector4::isNaN)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Rect>("Rect")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Rect();
                if (lua_gettop(L) == 6)
                    return new (ptr) Rect(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addProperty("x", &Rect::getX, &Rect::setX)
        .addProperty("y", &Rect::getY, &Rect::setY)
        .addProperty("width", &Rect::getWidth, &Rect::setWidth)
        .addProperty("height", &Rect::getHeight, &Rect::setHeight)
        .addFunction("__tostring", &Rect::toString)
        .addFunction("__eq", &Rect::operator==)
        .addFunction("getVector", &Rect::getVector)
        .addFunction("setRect", (void (Rect::*)(float, float, float, float))&Rect::setRect)
        .addFunction("fitOnRect", &Rect::fitOnRect)
        .addFunction("isNormalized", &Rect::isNormalized)
        .addFunction("isZero", &Rect::isZero)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Matrix3>("Matrix3")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Matrix3();
                if (lua_gettop(L) == 11)
                    return new (ptr) Matrix3(
                        luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), 
                        luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7), 
                        luaL_checknumber(L, 8), luaL_checknumber(L, 9), luaL_checknumber(L, 10)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
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
        .addStaticFunction("rotateMatrix", (Matrix3 (*)(const float, const Vector3&))&Matrix3::rotateMatrix)
        .addStaticFunction("rotateXMatrix", &Matrix3::rotateXMatrix)
        .addStaticFunction("rotateYMatrix", &Matrix3::rotateYMatrix)
        .addStaticFunction("rotateZMatrix", &Matrix3::rotateZMatrix)
        .addStaticFunction("scaleMatrix", (Matrix3 (*)(const Vector3&))&Matrix3::scaleMatrix)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Matrix4>("Matrix4")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Matrix4();
                if (lua_gettop(L) == 18)
                    return new (ptr) Matrix4(
                        luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5),
                        luaL_checknumber(L, 6), luaL_checknumber(L, 7), luaL_checknumber(L, 8), luaL_checknumber(L, 9),
                        luaL_checknumber(L, 10), luaL_checknumber(L, 11), luaL_checknumber(L, 12), luaL_checknumber(L, 13),
                        luaL_checknumber(L, 14), luaL_checknumber(L, 15), luaL_checknumber(L, 16), luaL_checknumber(L, 17)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
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
        .addStaticFunction("translateMatrix", (Matrix4 (*)(const Vector3&))&Matrix4::translateMatrix)
        .addStaticFunction("rotateMatrix", (Matrix4 (*)(const float, const Vector3&))&Matrix4::rotateMatrix)
        .addStaticFunction("rotateXMatrix", &Matrix4::rotateXMatrix)
        .addStaticFunction("rotateYMatrix", &Matrix4::rotateYMatrix)
        .addStaticFunction("rotateZMatrix", &Matrix4::rotateZMatrix)
        .addStaticFunction("scaleMatrix", (Matrix4 (*)(const Vector3&))&Matrix4::scaleMatrix)
        .addStaticFunction("lookAtMatrix", &Matrix4::lookAtMatrix)
        .addStaticFunction("frustumMatrix", &Matrix4::frustumMatrix)
        .addStaticFunction("orthoMatrix", &Matrix4::orthoMatrix)
        .addStaticFunction("perspectiveMatrix", &Matrix4::perspectiveMatrix)
        .addFunction("decompose", &Matrix4::decompose)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Quaternion>("Quaternion")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Quaternion();
                if (lua_gettop(L) == 6)
                    return new (ptr) Quaternion(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addFunction("__tostring", &Quaternion::toString)
        .addFunction("__eq", &Quaternion::operator==)
        .addFunction("__sub", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator-)
        .addFunction("__add", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator+)
        .addFunction("__mul", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator*)
        .addFunction("__mul", (Quaternion (Quaternion::*)(const Quaternion&) const)&Quaternion::operator*)
        .addFunction("__unm", (Quaternion (Quaternion::*)() const)&Quaternion::operator-)
        .addFunction("fromAxes", (void (Quaternion::*)(const Vector3&, const Vector3&, const Vector3&))&Quaternion::fromAxes)
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
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Plane();
                if (lua_gettop(L) == 4)
                    return new (ptr) Plane(
                        luabridge::Stack<Vector3>::get(L, 2),
                        luabridge::Stack<Vector3>::get(L, 3));
                if (lua_gettop(L) == 5)
                    return new (ptr) Plane(
                        luabridge::Stack<Vector3>::get(L, 2),
                        luabridge::Stack<Vector3>::get(L, 3),
                        luabridge::Stack<Vector3>::get(L, 4)); 
                if (lua_gettop(L) == 6)
                    return new (ptr) Plane(
                        luaL_checknumber(L, 2), luaL_checknumber(L, 3), 
                        luaL_checknumber(L, 4), luaL_checknumber(L, 5)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addFunction("__unm", (Plane (Plane::*)() const)&Plane::operator-)
        .addFunction("__eq", &Plane::operator==)
        .addFunction("getSide", (Plane::Side (Plane::*)(const Vector3&) const)&Plane::getSide)
        .addFunction("getDistance", &Plane::getDistance)
        .addFunction("redefine", (void (Plane::*)(const Vector3&, const Vector3&, const Vector3&))&Plane::redefine)
        .addFunction("projectVector", &Plane::projectVector)
        .addFunction("normalize", &Plane::normalize)
        .addStaticProperty("Side", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addProperty("NO_SIDE", Plane::Side::NO_SIDE)
                .addProperty("POSITIVE_SIDE", Plane::Side::POSITIVE_SIDE)
                .addProperty("NEGATIVE_SIDE", Plane::Side::NEGATIVE_SIDE)
                .addProperty("BOTH_SIDE", Plane::Side::BOTH_SIDE);
            table.pop();
            return table;
        })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<AlignedBox>("AlignedBox")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) AlignedBox();
                if (lua_gettop(L) == 4)
                    return new (ptr) AlignedBox(
                        luabridge::Stack<Vector3>::get(L, 2),
                        luabridge::Stack<Vector3>::get(L, 3)); 
                if (lua_gettop(L) == 8)
                    return new (ptr) AlignedBox(
                        luaL_checknumber(L, 2), luaL_checknumber(L, 3), 
                        luaL_checknumber(L, 4), luaL_checknumber(L, 5), 
                        luaL_checknumber(L, 6), luaL_checknumber(L, 7)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
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
        .addFunction("setExtents", (void(AlignedBox::*)(const Vector3&, const Vector3&))&AlignedBox::setExtents)
        .addFunction("getAllCorners", &AlignedBox::getAllCorners)
        .addFunction("getCorner", &AlignedBox::getCorner)
        .addFunction("merge", +[](AlignedBox* self, lua_State* L) -> void { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<AlignedBox>::isInstance(L, -1)) self->merge(luabridge::Stack<AlignedBox>::get(L, -1));
            else if (luabridge::Stack<Vector3>::isInstance(L, -1)) self->merge(luabridge::Stack<Vector3>::get(L, -1));
            else throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("transform", &AlignedBox::transform)
        .addFunction("isNull", &AlignedBox::isNull)
        .addFunction("setNull", &AlignedBox::setNull)
        .addFunction("isFinite", &AlignedBox::isFinite)
        .addFunction("isInfinite", &AlignedBox::isInfinite)
        .addFunction("setInfinite", &AlignedBox::isNull)
        .addFunction("intersects", +[](AlignedBox* self, lua_State* L) -> bool { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<AlignedBox>::isInstance(L, -1)) return self->intersects(luabridge::Stack<AlignedBox>::get(L, -1));
            if (luabridge::Stack<Plane>::isInstance(L, -1)) return self->intersects(luabridge::Stack<Plane>::get(L, -1));
            if (luabridge::Stack<Vector3>::isInstance(L, -1)) return self->intersects(luabridge::Stack<Vector3>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("intersection", &AlignedBox::intersection)
        .addFunction("volume", &AlignedBox::volume)
        .addFunction("scale", &AlignedBox::scale)
        .addFunction("getCenter", &AlignedBox::getCenter)
        .addFunction("getSize", &AlignedBox::getSize)
        .addFunction("getHalfSize", &AlignedBox::getHalfSize)
        .addFunction("contains", +[](AlignedBox* self, lua_State* L) -> bool { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<AlignedBox>::isInstance(L, -1)) return self->contains(luabridge::Stack<AlignedBox>::get(L, -1));
            if (luabridge::Stack<Vector3>::isInstance(L, -1)) return self->contains(luabridge::Stack<Vector3>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("squaredDistance", &AlignedBox::squaredDistance)
        .addFunction("distance", &AlignedBox::distance)
        .addStaticProperty("BoxType", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addProperty("BOXTYPE_NULL", AlignedBox::BoxType::BOXTYPE_NULL)
                .addProperty("BOXTYPE_FINITE", AlignedBox::BoxType::BOXTYPE_FINITE)
                .addProperty("BOXTYPE_INFINITE", AlignedBox::BoxType::BOXTYPE_INFINITE);
            table.pop();
            return table;
            })
        .addStaticProperty("CornerEnum", [](lua_State* L) {
            auto table = luabridge::newTable(L);
            table.push(L);
            luabridge::getNamespaceFromStack(L)
                .addProperty("FAR_LEFT_BOTTOM", AlignedBox::CornerEnum::FAR_LEFT_BOTTOM)
                .addProperty("FAR_LEFT_TOP", AlignedBox::CornerEnum::FAR_LEFT_TOP)
                .addProperty("FAR_RIGHT_TOP", AlignedBox::CornerEnum::FAR_RIGHT_TOP)
                .addProperty("FAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::FAR_RIGHT_BOTTOM)
                .addProperty("NEAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM)
                .addProperty("NEAR_LEFT_BOTTOM", AlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)
                .addProperty("NEAR_LEFT_TOP", AlignedBox::CornerEnum::NEAR_LEFT_TOP)
                .addProperty("NEAR_RIGHT_TOP", AlignedBox::CornerEnum::NEAR_RIGHT_TOP);
            table.pop();
            return table;
            })
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Ray>("Ray")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Ray();
                if (lua_gettop(L) == 4)
                    return new (ptr) Ray(
                        luabridge::Stack<Vector3>::get(L, 2),
                        luabridge::Stack<Vector3>::get(L, 3));
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addProperty("origin", &Ray::getOrigin, &Ray::setOrigin)
        .addProperty("direction", &Ray::getDirection, &Ray::setDirection)
        .addFunction("getPoint", &Ray::getPoint)
        .addFunction("intersects", +[](Ray* self, lua_State* L) -> float { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<AlignedBox>::isInstance(L, -1)) return self->intersects(luabridge::Stack<AlignedBox>::get(L, -1));
            if (luabridge::Stack<Plane>::isInstance(L, -1)) return self->intersects(luabridge::Stack<Plane>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addFunction("intersectionPoint", +[](Ray* self, lua_State* L) -> Vector3 { 
            if (lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (luabridge::Stack<AlignedBox>::isInstance(L, -1)) return self->intersectionPoint(luabridge::Stack<AlignedBox>::get(L, -1));
            if (luabridge::Stack<Plane>::isInstance(L, -1)) return self->intersectionPoint(luabridge::Stack<Plane>::get(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}