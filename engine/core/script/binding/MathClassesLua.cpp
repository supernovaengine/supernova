//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

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

void LuaBinding::registerMathClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS
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