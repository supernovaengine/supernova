//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "Text.h"

using namespace Supernova;

void LuaBinding::registerObjectUIClasses(lua_State *L){
    sol::state_view lua(L);


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