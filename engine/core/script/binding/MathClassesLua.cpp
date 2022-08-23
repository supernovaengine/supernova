//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

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
    sol::state_view lua(L);

    lua.new_usertype<Vector2>("Vector2",
        sol::constructors<Vector2(), Vector2(float, float)>(),
        "x", &Vector2::x,
        "y", &Vector2::y,
        sol::meta_function::to_string, &Vector2::toString,
        sol::meta_function::index, [](Vector2& v, const int index) { if (index < 0 || index > 1) return 0.0f; return v[index]; },
        sol::meta_function::new_index, [](Vector2& v, const int index, double x) { if (index < 0 || index > 1) return; v[index] = x; },
        sol::meta_function::equal_to, &Vector2::operator==,
        sol::meta_function::less_than, &Vector2::operator<,
        sol::meta_function::subtraction, sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator-),
        sol::meta_function::addition, sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator+),
        sol::meta_function::division, sol::resolve<Vector2(float) const>(&Vector2::operator/),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Vector2(const Vector2&) const>(&Vector2::operator*), sol::resolve<Vector2(float) const>(&Vector2::operator*) ),
        sol::meta_function::unary_minus, sol::resolve<Vector2() const>(&Vector2::operator-),
        "swap", &Vector2::swap,
        "length", &Vector2::length,
        "squaredLength", &Vector2::squaredLength,
        "distance", &Vector2::distance,
        "squaredDistance", &Vector2::squaredDistance,
        "dotProduct", &Vector2::dotProduct,
        "normalize", &Vector2::normalize,
        "midPoint", &Vector2::midPoint,
        "makeFloor", &Vector2::makeFloor,
        "makeCeil", &Vector2::makeCeil,
        "perpendicular", &Vector2::perpendicular,
        "crossProduct", &Vector2::crossProduct,
        "normalizedCopy", &Vector2::normalizedCopy,
        "reflect", &Vector2::reflect
        );

    lua.new_usertype<Vector3>("Vector3",
        sol::constructors<Vector3(), Vector3(float, float, float)>(),
        "x", &Vector3::x,
        "y", &Vector3::y,
        "z", &Vector3::z,
        sol::meta_function::to_string, &Vector3::toString,
        sol::meta_function::index, [](Vector3& v, const int index) { if (index < 0 || index > 2) return 0.0f; return v[index]; },
        sol::meta_function::new_index, [](Vector3& v, const int index, double x) { if (index < 0 || index > 2) return; v[index] = x; },
        sol::meta_function::equal_to, &Vector3::operator==,
        sol::meta_function::less_than, &Vector3::operator<,
        sol::meta_function::subtraction, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator-),
        sol::meta_function::addition, sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator+),
        sol::meta_function::division, sol::resolve<Vector3(float) const>(&Vector3::operator/),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Vector3(const Vector3&) const>(&Vector3::operator*), sol::resolve<Vector3(float) const>(&Vector3::operator*) ),
        sol::meta_function::unary_minus, sol::resolve<Vector3() const>(&Vector3::operator-),
        //sol::meta_function::modulus, &Vector3::operator%
        "length", &Vector3::length,
        "squaredLength", &Vector3::squaredLength,
        "dotProduct", &Vector3::dotProduct,
        "absDotProduct", &Vector3::absDotProduct,
        "distance", &Vector3::distance,
        "squaredDistance", &Vector3::squaredDistance,
        "normalize", &Vector3::normalize,
        "normalizeL", &Vector3::normalizeL,
        "crossProduct", &Vector3::crossProduct,
        "midPoint", &Vector3::midPoint,
        "makeFloor", &Vector3::makeFloor,
        "makeCeil", &Vector3::makeCeil,
        "perpendicular", &Vector3::perpendicular
        );

    lua.new_usertype<Vector4>("Vector4",
        sol::constructors<Vector4(), Vector4(float, float, float, float)>(),
        "x", &Vector4::x,
        "y", &Vector4::y,
        "z", &Vector4::z,
        "w", &Vector4::w,
        sol::meta_function::to_string, &Vector4::toString,
        sol::meta_function::index, [](Vector4& v, const int index) { if (index < 0 || index > 2) return 0.0f; return v[index]; },
        sol::meta_function::new_index, [](Vector4& v, const int index, double x) { if (index < 0 || index > 2) return; v[index] = x; },
        sol::meta_function::equal_to, &Vector4::operator==,
        sol::meta_function::less_than, &Vector4::operator<,
        sol::meta_function::subtraction, sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator-),
        sol::meta_function::addition, sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator+),
        sol::meta_function::division, sol::resolve<Vector4(float) const>(&Vector4::operator/),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Vector4(const Vector4&) const>(&Vector4::operator*), sol::resolve<Vector4(float) const>(&Vector4::operator*) ),
        sol::meta_function::unary_minus, sol::resolve<Vector4() const>(&Vector4::operator-),
        "swap", &Vector4::swap,
        "divideByW", &Vector4::divideByW,
        "dotProduct", &Vector4::dotProduct,
        "isNaN", &Vector4::isNaN
        );

    lua.new_usertype<Rect>("Rect",
        sol::constructors<Rect(), Rect(float, float, float, float)>(),
        "x", sol::property(&Rect::getX),
        "y", sol::property(&Rect::getY),
        "width", sol::property(&Rect::getWidth),
        "height", sol::property(&Rect::getHeight),
        sol::meta_function::to_string, &Rect::toString,
        sol::meta_function::equal_to, &Rect::operator==,
        "getVector", &Rect::getVector,
        "setRect", sol::overload( sol::resolve<void(float, float, float, float)>(&Rect::setRect), sol::resolve<void(Rect)>(&Rect::setRect) ),
        "fitOnRect", &Rect::fitOnRect,
        "isNormalized", &Rect::isNormalized,
        "isZero", &Rect::isZero
        );

    lua.new_usertype<Matrix3>("Matrix3",
        sol::constructors<Matrix3(), Matrix3(float, float, float, float, float, float, float, float, float)>(),
        sol::meta_function::to_string, &Matrix3::toString,
        //sol::meta_function::index, [](Matrix3& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
        //sol::meta_function::new_index, [](Matrix3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
        sol::meta_function::equal_to, &Matrix3::operator==,
        sol::meta_function::subtraction, sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator-),
        sol::meta_function::addition, sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator+),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Matrix3(const Matrix3&) const>(&Matrix3::operator*), sol::resolve<Vector3(const Vector3 &) const>(&Matrix3::operator*) ),
        "row", &Matrix3::row,
        "column", &Matrix3::column,
        "set", &Matrix3::set,
        "get", &Matrix3::get,
        "setRow", &Matrix3::setRow,
        "setColumn", &Matrix3::setColumn,
        "identity", &Matrix3::identity,
        "calcInverse", &Matrix3::calcInverse,
        "inverse", &Matrix3::inverse,
        "transpose", &Matrix3::transpose,
        "rotateMatrix", sol::overload( sol::resolve<Matrix3(const float, const Vector3 &)>(&Matrix3::rotateMatrix), sol::resolve<Matrix3(const float, const float)>(&Matrix3::rotateMatrix) ),
        "rotateXMatrix", &Matrix3::rotateXMatrix,
        "rotateYMatrix", &Matrix3::rotateYMatrix,
        "rotateZMatrix", &Matrix3::rotateZMatrix,
        "scaleMatrix", sol::overload( sol::resolve<Matrix3(const float)>(&Matrix3::scaleMatrix), sol::resolve<Matrix3(const Vector3&)>(&Matrix3::scaleMatrix) )
        );

    lua.new_usertype<Matrix4>("Matrix4",
        sol::constructors<Matrix4(), Matrix4(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)>(),
        sol::meta_function::to_string, &Matrix4::toString,
        sol::meta_function::equal_to, &Matrix4::operator==,
        sol::meta_function::subtraction, sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator-),
        sol::meta_function::addition, sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator+),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Matrix4(const Matrix4&) const>(&Matrix4::operator*), sol::resolve<Vector3(const Vector3 &) const>(&Matrix4::operator*), sol::resolve<Vector4(const Vector4 &) const>(&Matrix4::operator*) ),
        "row", &Matrix4::row,
        "column", &Matrix4::column,
        "set", &Matrix4::set,
        "get", &Matrix4::get,
        "setRow", &Matrix4::setRow,
        "setColumn", &Matrix4::setColumn,
        "identity", &Matrix4::identity,
        "translateInPlace", &Matrix4::translateInPlace,
        "inverse", &Matrix4::inverse,
        "transpose", &Matrix4::transpose,
        "determinant", &Matrix4::determinant,
        "translateMatrix", sol::overload( sol::resolve<Matrix4(float, float, float)>(&Matrix4::translateMatrix), sol::resolve<Matrix4(const Vector3&)>(&Matrix4::translateMatrix) ),
        "rotateMatrix", sol::overload( sol::resolve<Matrix4(const float, const Vector3 &)>(&Matrix4::rotateMatrix), sol::resolve<Matrix4(const float, const float)>(&Matrix4::rotateMatrix) ),
        "rotateXMatrix", &Matrix4::rotateXMatrix,
        "rotateYMatrix", &Matrix4::rotateYMatrix,
        "rotateZMatrix", &Matrix4::rotateZMatrix,
        "scaleMatrix", sol::overload( sol::resolve<Matrix4(const float)>(&Matrix4::scaleMatrix), sol::resolve<Matrix4(const Vector3&)>(&Matrix4::scaleMatrix) ),
        "lookAtMatrix", &Matrix4::lookAtMatrix,
        "frustumMatrix", &Matrix4::frustumMatrix,
        "orthoMatrix", &Matrix4::orthoMatrix,
        "perspectiveMatrix", &Matrix4::perspectiveMatrix,
        "decompose", &Matrix4::decompose
        );

    lua.new_usertype<Quaternion>("Quaternion",
        sol::constructors<Quaternion(), Quaternion(float, float, float, float)>(),
        sol::meta_function::to_string, &Quaternion::toString,
        sol::meta_function::index, [](Quaternion& q, const int index) { if (index < 0 || index > 2) return 0.0f; return q[index]; },
        sol::meta_function::new_index, [](Quaternion& q, const int index, double x) { if (index < 0 || index > 2) return; q[index] = x; },
        sol::meta_function::equal_to, &Quaternion::operator==,
        sol::meta_function::subtraction, sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator-),
        sol::meta_function::addition, sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator+),
        sol::meta_function::multiplication, sol::overload( sol::resolve<Quaternion(const Quaternion&) const>(&Quaternion::operator*), sol::resolve<Quaternion(float) const>(&Quaternion::operator*) ),
        sol::meta_function::unary_minus, sol::resolve<Quaternion() const>(&Quaternion::operator-),
        "fromAxes", sol::resolve<void(const Vector3&, const Vector3&, const Vector3&)>(&Quaternion::fromAxes),
        "fromRotationMatrix", &Quaternion::fromRotationMatrix,
        "getRotationMatrix", &Quaternion::getRotationMatrix,
        "fromAngle", &Quaternion::fromAngle,
        "fromAngleAxis", &Quaternion::fromAngleAxis,
        "xAxis", &Quaternion::xAxis,
        "yAxis", &Quaternion::yAxis,
        "zAxis", &Quaternion::zAxis,
        "dot", &Quaternion::dot,
        "norm", &Quaternion::norm,
        "inverse", &Quaternion::inverse,
        "unitInverse", &Quaternion::unitInverse,
        "exp", &Quaternion::exp,
        "log", &Quaternion::log,
        "slerp", &Quaternion::slerp,
        "slerpExtraSpins", &Quaternion::slerpExtraSpins,
        "squad", &Quaternion::squad,
        "nlerp", &Quaternion::nlerp,
        "normalise", &Quaternion::normalise,
        "getRoll", &Quaternion::getRoll,
        "getPitch", &Quaternion::getPitch,
        "getYaw", &Quaternion::getYaw
        );

    lua.new_usertype<Plane>("Plane",
        sol::constructors<Plane(), Plane(const Vector3&, float), Plane(float, float, float, float), Plane(const Vector3&, const Vector3&), Plane(const Vector3&, const Vector3&, const Vector3&)>(),
        sol::meta_function::unary_minus, sol::resolve<Plane() const>(&Plane::operator-),
        sol::meta_function::equal_to, &Plane::operator==,
        "side", sol::property( sol::resolve<Plane::Side(const Vector3&) const>(&Plane::getSide) ),
        "getSide", sol::overload( sol::resolve<Plane::Side(const Vector3&) const>(&Plane::getSide), sol::resolve<Plane::Side(const Vector3&, const Vector3&) const>(&Plane::getSide), sol::resolve<Plane::Side(const AlignedBox&) const>(&Plane::getSide) ),
        "distance", sol::property( &Plane::getDistance ),
        "getDistance", &Plane::getDistance,
        "redefine", sol::overload( sol::resolve<void(const Vector3&, const Vector3&, const Vector3&)>(&Plane::redefine), sol::resolve<void(const Vector3&, const Vector3&)>(&Plane::redefine) ),
        "projectVector", &Plane::projectVector,
        "normalize", &Plane::normalize,
        "Side", lua.create_table_with(
            "NO_SIDE", Plane::Side::NO_SIDE,
            "POSITIVE_SIDE", Plane::Side::POSITIVE_SIDE,
            "NEGATIVE_SIDE", Plane::Side::NEGATIVE_SIDE,
            "BOTH_SIDE", Plane::Side::BOTH_SIDE
            )
        );

    lua.new_usertype<AlignedBox>("AlignedBox",
        sol::constructors<AlignedBox(), AlignedBox(AlignedBox::BoxType), AlignedBox(const Vector3&, const Vector3&), AlignedBox(float, float, float, float, float, float)>(),
        sol::meta_function::equal_to, &AlignedBox::operator==,
        "minimum", sol::property( sol::resolve<Vector3&()>(&AlignedBox::getMinimum), sol::resolve<void(const Vector3&)>(&AlignedBox::setMinimum) ),
        "getMinimum",  sol::resolve<Vector3&()>(&AlignedBox::getMinimum),
        "setMinimum", sol::overload( sol::resolve<void(const Vector3&)>(&AlignedBox::setMinimum), sol::resolve<void(float, float, float)>(&AlignedBox::setMinimum) ),
        "setMinimumX", &AlignedBox::setMinimumX,
        "setMinimumY", &AlignedBox::setMinimumY,
        "setMinimumZ", &AlignedBox::setMinimumZ,
        "maximum", sol::property( sol::resolve<Vector3&()>(&AlignedBox::getMaximum), sol::resolve<void(const Vector3&)>(&AlignedBox::setMaximum) ),
        "getMaximum",  sol::resolve<Vector3&()>(&AlignedBox::getMaximum),
        "setMaximum", sol::overload( sol::resolve<void(const Vector3&)>(&AlignedBox::setMaximum), sol::resolve<void(float, float, float)>(&AlignedBox::setMaximum) ),
        "setMaximumX", &AlignedBox::setMaximumX,
        "setMaximumY", &AlignedBox::setMaximumY,
        "setMaximumZ", &AlignedBox::setMaximumZ,
        "setExtents", sol::overload( sol::resolve<void(const Vector3&, const Vector3&)>(&AlignedBox::setExtents), sol::resolve<void(float, float, float, float, float, float)>(&AlignedBox::setExtents) ),
        "getAllCorners", &AlignedBox::getAllCorners,
        "getCorner", &AlignedBox::getCorner,
        "merge", sol::overload( sol::resolve<void(const AlignedBox&)>(&AlignedBox::merge), sol::resolve<void(const Vector3&)>(&AlignedBox::merge) ),
        "transform", &AlignedBox::transform,
        "null", sol::property( &AlignedBox::isNull, &AlignedBox::setNull ),
        "setNull", &AlignedBox::setNull,
        "isNull", &AlignedBox::isNull,
        "finite", sol::property( &AlignedBox::isFinite ),
        "isFinite", &AlignedBox::isFinite,
        "infinite", sol::property( &AlignedBox::isInfinite, &AlignedBox::setInfinite ),
        "setInfinite", &AlignedBox::setInfinite,
        "isInfinite", &AlignedBox::isInfinite,
        "intersects", sol::overload( sol::resolve<bool(const AlignedBox&) const>(&AlignedBox::intersects), sol::resolve<bool(const Plane&) const>(&AlignedBox::intersects), sol::resolve<bool(const Vector3&) const>(&AlignedBox::intersects) ),
        "intersection", &AlignedBox::intersection,
        "volume", &AlignedBox::volume,
        "scale", &AlignedBox::scale,
        "getCenter", &AlignedBox::getCenter,
        "getSize", &AlignedBox::getSize,
        "getHalfSize", &AlignedBox::getHalfSize,
        "contains", sol::overload( sol::resolve<bool(const Vector3&) const>(&AlignedBox::contains), sol::resolve<bool(const AlignedBox&) const>(&AlignedBox::contains) ),
        "squaredDistance", &AlignedBox::squaredDistance,
        "distance", &AlignedBox::distance,
        "BoxType", lua.create_table_with(
            "BOXTYPE_NULL", AlignedBox::BoxType::BOXTYPE_NULL,
            "BOXTYPE_FINITE", AlignedBox::BoxType::BOXTYPE_FINITE,
            "BOXTYPE_INFINITE", AlignedBox::BoxType::BOXTYPE_INFINITE
            ),
        "CornerEnum", lua.create_table_with(
            "FAR_LEFT_BOTTOM", AlignedBox::CornerEnum::FAR_LEFT_BOTTOM,
            "FAR_LEFT_TOP", AlignedBox::CornerEnum::FAR_LEFT_TOP,
            "FAR_RIGHT_TOP", AlignedBox::CornerEnum::FAR_RIGHT_TOP,
            "FAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::FAR_RIGHT_BOTTOM,
            "NEAR_RIGHT_BOTTOM", AlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM,
            "NEAR_LEFT_BOTTOM", AlignedBox::CornerEnum::NEAR_LEFT_BOTTOM,
            "NEAR_LEFT_TOP", AlignedBox::CornerEnum::NEAR_LEFT_TOP,
            "NEAR_RIGHT_TOP", AlignedBox::CornerEnum::NEAR_RIGHT_TOP
            )
        );


    lua.new_usertype<Ray>("Ray",
        sol::constructors<Ray(), Vector4(Vector3, Vector3)>(),
        "origin", sol::property(&Ray::getOrigin, &Ray::setOrigin),
        "setOrigin", &Ray::setOrigin,
        "getOrigin", &Ray::getOrigin,
        "direction", sol::property(&Ray::getDirection, &Ray::setDirection),
        "setDirection", &Ray::setDirection,
        "getDirection", &Ray::getDirection,
        "point", sol::property(&Ray::getPoint, &Ray::setOrigin),
        "getPoint", &Ray::getPoint,
        "intersects", sol::overload( sol::resolve<float(Plane)>(&Ray::intersects), sol::resolve<float(AlignedBox)>(&Ray::intersects) ),
        "intersectionPoint", sol::overload( sol::resolve<Vector3(Plane)>(&Ray::intersectionPoint), sol::resolve<Vector3(AlignedBox)>(&Ray::intersectionPoint) )
        );

}