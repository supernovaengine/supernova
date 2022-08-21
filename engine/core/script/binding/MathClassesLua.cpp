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
#include "Rect.h"

using namespace Supernova;

void LuaBinding::registerMathClasses(lua_State *L){
    sol::state_view lua(L);

    lua.new_usertype<Vector2>("Vector2",
        sol::constructors<Vector2(float, float)>(),
        "x", &Vector2::x,
        "y", &Vector2::y
        );

    lua.new_usertype<Vector3>("Vector3",
        sol::constructors<Vector3(float, float, float)>(),
        "x", &Vector3::x,
        "y", &Vector3::y,
        "z", &Vector3::z,
        sol::meta_function::to_string, &Vector3::toString,
        sol::meta_function::index, [](Vector3& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
        sol::meta_function::new_index, [](Vector3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
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
        sol::constructors<Vector4(float, float, float, float)>(),
        "x", &Vector4::x,
        "y", &Vector4::y,
        "z", &Vector4::z,
        "w", &Vector4::w
        );

    lua.new_usertype<Rect>("Rect",
        sol::constructors<Rect(), Rect(float, float, float, float)>(),
        "x", sol::property(&Rect::getX),
        "y", sol::property(&Rect::getY),
        "width", sol::property(&Rect::getWidth),
        "height", sol::property(&Rect::getHeight)
        );
}