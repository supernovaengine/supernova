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
        .endClass();

/*
    sol::state_view lua(L);

    auto vector2 = lua.new_usertype<Vector2>("Vector2",
        sol::call_constructor, sol::constructors<Vector2(), Vector2(float, float)>());
        
    vector2["x"] = &Vector2::x;
    vector2["y"] = &Vector2::y;
    vector2[sol::meta_function::to_string] = &Vector2::toString;
    vector2[sol::meta_function::index] = [](Vector2& v, const int index) { if (index < 0 || index > 1) return 0.0f; return v[index]; };
    vector2[sol::meta_function::new_index] = [](Vector2& v, const int index, double x) { if (index < 0 || index > 1) return; v[index] = x; };
    vector2[sol::meta_function::equal_to] = &Vector2::operator==;
    vector2[sol::meta_function::less_than] = &Vector2::operator<;
    vector2[sol::meta_function::subtraction] = sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator-);
    vector2[sol::meta_function::addition] = sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator+);
    vector2[sol::meta_function::division] = sol::resolve<Vector2(float) const>(&Vector2::operator/);
    vector2[sol::meta_function::multiplication] = sol::overload( sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator*), sol::resolve<Vector2(float) const>(&Vector2::operator*) );
    vector2[sol::meta_function::unary_minus] = sol::resolve<Vector2() const>(&Vector2::operator-);
    vector2["swap"] = &Vector2::swap;
    vector2["length"] = &Vector2::length;
    vector2["squaredLength"] = &Vector2::squaredLength;
    vector2["distance"] = &Vector2::distance;
    vector2["squaredDistance"] = &Vector2::squaredDistance;
    vector2["dotProduct"] = &Vector2::dotProduct;
    vector2["normalize"] = &Vector2::normalize;
    vector2["midPoint"] = &Vector2::midPoint;
    vector2["makeFloor"] = &Vector2::makeFloor;
    vector2["makeCeil"] = &Vector2::makeCeil;
    vector2["perpendicular"] = &Vector2::perpendicular;
    vector2["crossProduct"] = &Vector2::crossProduct;
    vector2["normalizedCopy"] = &Vector2::normalizedCopy;
    vector2["reflect"] = &Vector2::reflect;

    auto vector3 = lua.new_usertype<Vector3>("Vector3",
        sol::call_constructor, sol::constructors<Vector3(), Vector3(float, float, float)>());

    vector3["x"] = &Vector3::x;
    vector3["y"] = &Vector3::y;
    vector3["z"] = &Vector3::z;
    vector3[sol::meta_function::to_string] = &Vector3::toString;
    vector3[sol::meta_function::index] = [](Vector3& v, const int index) { if (index < 0 || index > 2) return 0.0f; return v[index]; };
    vector3[sol::meta_function::new_index] = [](Vector3& v, const int index, double x) { if (index < 0 || index > 2) return; v[index] = x; };
    vector3[sol::meta_function::equal_to] = &Vector3::operator==;
    vector3[sol::meta_function::less_than] = &Vector3::operator<;
    vector3[sol::meta_function::subtraction] = sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator-);
    vector3[sol::meta_function::addition] = sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator+);
    vector3[sol::meta_function::division] = sol::resolve<Vector3(float) const>(&Vector3::operator/);
    vector3[sol::meta_function::multiplication] = sol::overload( sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator*), sol::resolve<Vector3(float) const>(&Vector3::operator*) );
    vector3[sol::meta_function::unary_minus] = sol::resolve<Vector3() const>(&Vector3::operator-);
    //vector3[sol::meta_function::modulus] = &Vector3::operator%;
    vector3["length"] = &Vector3::length;
    vector3["squaredLength"] = &Vector3::squaredLength;
    vector3["dotProduct"] = &Vector3::dotProduct;
    vector3["absDotProduct"] = &Vector3::absDotProduct;
    vector3["distance"] = &Vector3::distance;
    vector3["squaredDistance"] = &Vector3::squaredDistance;
    vector3["normalize"] = &Vector3::normalize;
    vector3["normalizeL"] = &Vector3::normalizeL;
    vector3["crossProduct"] = &Vector3::crossProduct;
    vector3["midPoint"] = &Vector3::midPoint;
    vector3["makeFloor"] = &Vector3::makeFloor;
    vector3["makeCeil"] = &Vector3::makeCeil;
    vector3["perpendicular"] = &Vector3::perpendicular;

    auto vector4 = lua.new_usertype<Vector4>("Vector4",
        sol::call_constructor, sol::constructors<Vector4(), Vector4(float, float, float, float)>());

    vector4["x"] = &Vector4::x;
    vector4["y"] = &Vector4::y;
    vector4["z"] = &Vector4::z;
    vector4["w"] = &Vector4::w;
    vector4[sol::meta_function::to_string] = &Vector4::toString;
    vector4[sol::meta_function::index] = [](Vector4& v, const int index) { if (index < 0 || index > 2) return 0.0f; return v[index]; };
    vector4[sol::meta_function::new_index] = [](Vector4& v, const int index, double x) { if (index < 0 || index > 2) return; v[index] = x; };
    vector4[sol::meta_function::equal_to] = &Vector4::operator==;
    vector4[sol::meta_function::less_than] = &Vector4::operator<;
    vector4[sol::meta_function::subtraction] = sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator-);
    vector4[sol::meta_function::addition] = sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator+);
    vector4[sol::meta_function::division] = sol::resolve<Vector4(float) const>(&Vector4::operator/);
    vector4[sol::meta_function::multiplication] = sol::overload( sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator*), sol::resolve<Vector4(float) const>(&Vector4::operator*) );
    vector4[sol::meta_function::unary_minus] = sol::resolve<Vector4() const>(&Vector4::operator-);
    vector4["swap"] = &Vector4::swap;
    vector4["divideByW"] = &Vector4::divideByW;
    vector4["dotProduct"] = &Vector4::dotProduct;
    vector4["isNaN"] = &Vector4::isNaN;

    auto rect = lua.new_usertype<Rect>("Rect",
        sol::call_constructor, sol::constructors<Rect(), Rect(float, float, float, float)>());

    rect["x"] = sol::property(&Rect::getX);
    rect["y"] = sol::property(&Rect::getY);
    rect["width"] = sol::property(&Rect::getWidth);
    rect["height"] = sol::property(&Rect::getHeight);
    rect[sol::meta_function::to_string] = &Rect::toString;
    rect[sol::meta_function::equal_to] = &Rect::operator==;
    rect["getVector"] = &Rect::getVector;
    rect["setRect"] = sol::overload( sol::resolve<void(float, float, float, float)>(&Rect::setRect), sol::resolve<void(Rect)>(&Rect::setRect) );
    rect["fitOnRect"] = &Rect::fitOnRect;
    rect["isNormalized"] = &Rect::isNormalized;
    rect["isZero"] = &Rect::isZero;

    auto matrix3 = lua.new_usertype<Matrix3>("Matrix3",
        sol::call_constructor, sol::constructors<Matrix3(), Matrix3(float, float, float, float, float, float, float, float, float)>());

    matrix3[sol::meta_function::to_string] = &Matrix3::toString;
    matrix3[sol::meta_function::equal_to] = &Matrix3::operator==;
    matrix3[sol::meta_function::subtraction] = sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator-);
    matrix3[sol::meta_function::addition] = sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator+);
    matrix3[sol::meta_function::multiplication] = sol::overload( sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator*), sol::resolve<Vector3(const Vector3 &) const>(&Matrix3::operator*) );
    matrix3["row"] = &Matrix3::row;
    matrix3["column"] = &Matrix3::column;
    matrix3["set"] = &Matrix3::set;
    matrix3["get"] = &Matrix3::get;
    matrix3["setRow"] = &Matrix3::setRow;
    matrix3["setColumn"] = &Matrix3::setColumn;
    matrix3["identity"] = &Matrix3::identity;
    matrix3["calcInverse"] = &Matrix3::calcInverse;
    matrix3["inverse"] = &Matrix3::inverse;
    matrix3["transpose"] = &Matrix3::transpose;
    matrix3["rotateMatrix"] = sol::overload( sol::resolve<Matrix3(const float, const Vector3 &)>(&Matrix3::rotateMatrix), sol::resolve<Matrix3(const float, const float)>(&Matrix3::rotateMatrix) );
    matrix3["rotateXMatrix"] = &Matrix3::rotateXMatrix;
    matrix3["rotateYMatrix"] = &Matrix3::rotateYMatrix;
    matrix3["rotateZMatrix"] = &Matrix3::rotateZMatrix;
    matrix3["scaleMatrix"] = sol::overload( sol::resolve<Matrix3(const float)>(&Matrix3::scaleMatrix), sol::resolve<Matrix3(const Vector3&)>(&Matrix3::scaleMatrix) );

    auto matrix4 = lua.new_usertype<Matrix4>("Matrix4",
        sol::call_constructor, sol::constructors<Matrix4(), Matrix4(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)>());

    matrix4[sol::meta_function::to_string] = &Matrix4::toString;
    matrix4[sol::meta_function::equal_to] = &Matrix4::operator==;
    matrix4[sol::meta_function::subtraction] = sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator-);
    matrix4[sol::meta_function::addition] = sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator+);
    matrix4[sol::meta_function::multiplication] = sol::overload( sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator*), sol::resolve<Vector3(const Vector3 &) const>(&Matrix4::operator*), sol::resolve<Vector4(const Vector4 &) const>(&Matrix4::operator*) );
    matrix4["row"] = &Matrix4::row;
    matrix4["column"] = &Matrix4::column;
    matrix4["set"] = &Matrix4::set;
    matrix4["get"] = &Matrix4::get;
    matrix4["setRow"] = &Matrix4::setRow;
    matrix4["setColumn"] = &Matrix4::setColumn;
    matrix4["identity"] = &Matrix4::identity;
    matrix4["translateInPlace"] = &Matrix4::translateInPlace;
    matrix4["inverse"] = &Matrix4::inverse;
    matrix4["transpose"] = &Matrix4::transpose;
    matrix4["determinant"] = &Matrix4::determinant;
    matrix4["translateMatrix"] = sol::overload( sol::resolve<Matrix4(float, float, float)>(&Matrix4::translateMatrix), sol::resolve<Matrix4(const Vector3&)>(&Matrix4::translateMatrix) );
    matrix4["rotateMatrix"] = sol::overload( sol::resolve<Matrix4(const float, const Vector3 &)>(&Matrix4::rotateMatrix), sol::resolve<Matrix4(const float, const float)>(&Matrix4::rotateMatrix) );
    matrix4["rotateXMatrix"] = &Matrix4::rotateXMatrix;
    matrix4["rotateYMatrix"] = &Matrix4::rotateYMatrix;
    matrix4["rotateZMatrix"] = &Matrix4::rotateZMatrix;
    matrix4["scaleMatrix"] = sol::overload( sol::resolve<Matrix4(const float)>(&Matrix4::scaleMatrix), sol::resolve<Matrix4(const Vector3&)>(&Matrix4::scaleMatrix) );
    matrix4["lookAtMatrix"] = &Matrix4::lookAtMatrix;
    matrix4["frustumMatrix"] = &Matrix4::frustumMatrix;
    matrix4["orthoMatrix"] = &Matrix4::orthoMatrix;
    matrix4["perspectiveMatrix"] = &Matrix4::perspectiveMatrix;
    matrix4["decompose"] = &Matrix4::decompose;

    auto quaternion = lua.new_usertype<Quaternion>("Quaternion",
        sol::call_constructor, sol::constructors<Quaternion(), Quaternion(float, float, float, float)>());

    quaternion[sol::meta_function::to_string] = &Quaternion::toString;
    quaternion[sol::meta_function::index] = [](Quaternion& q, const int index) { if (index < 0 || index > 2) return 0.0f; return q[index]; };
    quaternion[sol::meta_function::new_index] = [](Quaternion& q, const int index, double x) { if (index < 0 || index > 2) return; q[index] = x; };
    quaternion[sol::meta_function::equal_to] = &Quaternion::operator==;
    quaternion[sol::meta_function::subtraction] = sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator-);
    quaternion[sol::meta_function::addition] = sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator+);
    quaternion[sol::meta_function::multiplication] = sol::overload( sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator*), sol::resolve<Quaternion(float) const>(&Quaternion::operator*) );
    quaternion[sol::meta_function::unary_minus] = sol::resolve<Quaternion() const>(&Quaternion::operator-);
    quaternion["fromAxes"] = sol::resolve<void(const Vector3&, const Vector3&, const Vector3&)>(&Quaternion::fromAxes);
    quaternion["fromRotationMatrix"] = &Quaternion::fromRotationMatrix;
    quaternion["getRotationMatrix"] = &Quaternion::getRotationMatrix;
    quaternion["fromAngle"] = &Quaternion::fromAngle;
    quaternion["fromAngleAxis"] = &Quaternion::fromAngleAxis;
    quaternion["xAxis"] = &Quaternion::xAxis;
    quaternion["yAxis"] = &Quaternion::yAxis;
    quaternion["zAxis"] = &Quaternion::zAxis;
    quaternion["dot"] = &Quaternion::dot;
    quaternion["norm"] = &Quaternion::norm;
    quaternion["inverse"] = &Quaternion::inverse;
    quaternion["unitInverse"] = &Quaternion::unitInverse;
    quaternion["exp"] = &Quaternion::exp;
    quaternion["log"] = &Quaternion::log;
    quaternion["slerp"] = &Quaternion::slerp;
    quaternion["slerpExtraSpins"] = &Quaternion::slerpExtraSpins;
    quaternion["squad"] = &Quaternion::squad;
    quaternion["nlerp"] = &Quaternion::nlerp;
    quaternion["normalise"] = &Quaternion::normalise;
    quaternion["getRoll"] = &Quaternion::getRoll;
    quaternion["getPitch"] = &Quaternion::getPitch;
    quaternion["getYaw"] = &Quaternion::getYaw;

    auto plane = lua.new_usertype<Plane>("Plane",
        sol::call_constructor, sol::constructors<Plane(), Plane(const Vector3&, float), Plane(float, float, float, float), Plane(const Vector3&, const Vector3&), Plane(const Vector3&, const Vector3&, const Vector3&)>());

    plane[sol::meta_function::unary_minus] = sol::resolve<Plane() const>(&Plane::operator-);
    plane[sol::meta_function::equal_to] = &Plane::operator==;
    plane["side"] = sol::property( sol::resolve<Plane::Side(const Vector3&) const>(&Plane::getSide) );
    plane["getSide"] = sol::overload( sol::resolve<Plane::Side(const Vector3&) const>(&Plane::getSide), sol::resolve<Plane::Side(const Vector3&, const Vector3&) const>(&Plane::getSide), sol::resolve<Plane::Side(const AlignedBox&) const>(&Plane::getSide) );
    plane["distance"] = sol::property( &Plane::getDistance );
    plane["getDistance"] = &Plane::getDistance;
    plane["redefine"] = sol::overload( sol::resolve<void(const Vector3&, const Vector3&, const Vector3&)>(&Plane::redefine), sol::resolve<void(const Vector3&, const Vector3&)>(&Plane::redefine) );
    plane["projectVector"] = &Plane::projectVector;
    plane["normalize"] = &Plane::normalize;
    plane["Side"] = lua.create_table_with(
        "NO_SIDE", Plane::Side::NO_SIDE,
        "POSITIVE_SIDE", Plane::Side::POSITIVE_SIDE,
        "NEGATIVE_SIDE", Plane::Side::NEGATIVE_SIDE,
        "BOTH_SIDE", Plane::Side::BOTH_SIDE
        );

    auto alignedbox = lua.new_usertype<AlignedBox>("AlignedBox",
        sol::call_constructor, sol::constructors<AlignedBox(), AlignedBox(AlignedBox::BoxType), AlignedBox(const Vector3&, const Vector3&), AlignedBox(float, float, float, float, float, float)>(),
        sol::meta_function::equal_to, &AlignedBox::operator==);

    alignedbox["minimum"] = sol::property( sol::resolve<Vector3&()>(&AlignedBox::getMinimum), sol::resolve<void(const Vector3&)>(&AlignedBox::setMinimum) );
    alignedbox["getMinimum"] =  sol::resolve<Vector3&()>(&AlignedBox::getMinimum);
    alignedbox["setMinimum"] = sol::overload( sol::resolve<void(const Vector3&)>(&AlignedBox::setMinimum), sol::resolve<void(float, float, float)>(&AlignedBox::setMinimum) );
    alignedbox["setMinimumX"] = &AlignedBox::setMinimumX;
    alignedbox["setMinimumY"] = &AlignedBox::setMinimumY;
    alignedbox["setMinimumZ"] = &AlignedBox::setMinimumZ;
    alignedbox["maximum"] = sol::property( sol::resolve<Vector3&()>(&AlignedBox::getMaximum), sol::resolve<void(const Vector3&)>(&AlignedBox::setMaximum) );
    alignedbox["getMaximum"] =  sol::resolve<Vector3&()>(&AlignedBox::getMaximum);
    alignedbox["setMaximum"] = sol::overload( sol::resolve<void(const Vector3&)>(&AlignedBox::setMaximum), sol::resolve<void(float, float, float)>(&AlignedBox::setMaximum) );
    alignedbox["setMaximumX"] = &AlignedBox::setMaximumX;
    alignedbox["setMaximumY"] = &AlignedBox::setMaximumY;
    alignedbox["setMaximumZ"] = &AlignedBox::setMaximumZ;
    alignedbox["setExtents"] = sol::overload( sol::resolve<void(const Vector3&, const Vector3&)>(&AlignedBox::setExtents), sol::resolve<void(float, float, float, float, float, float)>(&AlignedBox::setExtents) );
    alignedbox["getAllCorners"] = &AlignedBox::getAllCorners;
    alignedbox["getCorner"] = &AlignedBox::getCorner;
    alignedbox["merge"] = sol::overload( sol::resolve<void(const AlignedBox&)>(&AlignedBox::merge), sol::resolve<void(const Vector3&)>(&AlignedBox::merge) );
    alignedbox["transform"] = &AlignedBox::transform;
    alignedbox["null"] = sol::property( &AlignedBox::isNull, &AlignedBox::setNull );
    alignedbox["setNull"] = &AlignedBox::setNull;
    alignedbox["isNull"] = &AlignedBox::isNull;
    alignedbox["finite"] = sol::property( &AlignedBox::isFinite );
    alignedbox["isFinite"] = &AlignedBox::isFinite;
    alignedbox["infinite"] = sol::property( &AlignedBox::isInfinite, &AlignedBox::setInfinite );
    alignedbox["setInfinite"] = &AlignedBox::setInfinite;
    alignedbox["isInfinite"] = &AlignedBox::isInfinite;
    alignedbox["intersects"] = sol::overload( sol::resolve<bool(const AlignedBox&) const>(&AlignedBox::intersects), sol::resolve<bool(const Plane&) const>(&AlignedBox::intersects), sol::resolve<bool(const Vector3&) const>(&AlignedBox::intersects) );
    alignedbox["intersection"] = &AlignedBox::intersection;
    alignedbox["volume"] = &AlignedBox::volume;
    alignedbox["scale"] = &AlignedBox::scale;
    alignedbox["getCenter"] = &AlignedBox::getCenter;
    alignedbox["getSize"] = &AlignedBox::getSize;
    alignedbox["getHalfSize"] = &AlignedBox::getHalfSize;
    alignedbox["contains"] = sol::overload( sol::resolve<bool(const Vector3&) const>(&AlignedBox::contains), sol::resolve<bool(const AlignedBox&) const>(&AlignedBox::contains) );
    alignedbox["squaredDistance"] = &AlignedBox::squaredDistance;
    alignedbox["distance"] = &AlignedBox::distance;
    alignedbox["BoxType"] = lua.create_table_with(
        "BOXTYPE_NULL", AlignedBox::BoxType::BOXTYPE_NULL,
        "BOXTYPE_FINITE", AlignedBox::BoxType::BOXTYPE_FINITE,
        "BOXTYPE_INFINITE", AlignedBox::BoxType::BOXTYPE_INFINITE
        );
    alignedbox["CornerEnum"] = lua.create_table_with(
        "FAR_LEFT_BOTTOM", AlignedBox::CornerEnum::FAR_LEFT_BOTTOM,
        "FAR_LEFT_TOP", AlignedBox::CornerEnum::FAR_LEFT_TOP,
        "FAR_RIGHT_TOP", AlignedBox::CornerEnum::FAR_RIGHT_TOP,
        "FAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::FAR_RIGHT_BOTTOM,
        "NEAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM,
        "NEAR_LEFT_BOTTOM", AlignedBox::CornerEnum::NEAR_LEFT_BOTTOM,
        "NEAR_LEFT_TOP", AlignedBox::CornerEnum::NEAR_LEFT_TOP,
        "NEAR_RIGHT_TOP", AlignedBox::CornerEnum::NEAR_RIGHT_TOP
        );


    auto ray = lua.new_usertype<Ray>("Ray",
        sol::call_constructor, sol::constructors<Ray(), Vector4(Vector3, Vector3)>());

    ray["origin"] = sol::property(&Ray::getOrigin, &Ray::setOrigin);
    ray["setOrigin"] = &Ray::setOrigin;
    ray["getOrigin"] = &Ray::getOrigin;
    ray["direction"] = sol::property(&Ray::getDirection, &Ray::setDirection);
    ray["setDirection"] = &Ray::setDirection;
    ray["getDirection"] = &Ray::getDirection;
    ray["point"] = sol::property(&Ray::getPoint, &Ray::setOrigin);
    ray["getPoint"] = &Ray::getPoint;
    ray["intersects"] = sol::overload( sol::resolve<float(Plane)>(&Ray::intersects), sol::resolve<float(AlignedBox)>(&Ray::intersects) );
    ray["intersectionPoint"] = sol::overload( sol::resolve<Vector3(Plane)>(&Ray::intersectionPoint), sol::resolve<Vector3(AlignedBox)>(&Ray::intersectionPoint) );
*/
#endif //DISABLE_LUA_BINDINGS
}