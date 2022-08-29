//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "action/Action.h"
#include "action/Animation.h"
#include "action/Ease.h"

using namespace Supernova;

void LuaBinding::registerActionClasses(lua_State *L){
    sol::state_view lua(L);

    lua.new_usertype<Action>("Action",
        sol::constructors<Action(Scene*),Action(Scene*, Entity)>(),
        "start", &Action::start,
        "pause", &Action::pause,
        "target", sol::property(&Action::getTarget, &Action::setTarget),
        "setTarget", &Action::setTarget,
        "getTarget", &Action::getTarget,
        "speed", sol::property(&Action::getSpeed, &Action::setSpeed),
        "setSpeed", &Action::setSpeed,
        "getSpeed", &Action::getSpeed,
        "entity", sol::property(&Action::getEntity),
        "getEntity", &Action::getEntity
        );

    lua.new_usertype<Animation>("Animation",
        sol::constructors<Animation(Scene*),Animation(Scene*, Entity)>(),
        sol::base_classes, sol::bases<Action>(),
        "loop", sol::property(&Animation::isLoop, &Animation::setLoop),
        "isLoop", &Animation::isLoop,
        "setLoop", &Animation::setLoop,
        "endTime", sol::property(&Animation::getEndTime, &Animation::setEndTime),
        "setEndTime", &Animation::setEndTime,
        "getEndTime", &Animation::getEndTime,
        "ownedActions", sol::property(&Animation::isOwnedActions, &Animation::setOwnedActions),
        "isOwnedActions", &Animation::isOwnedActions,
        "setOwnedActions", &Animation::setOwnedActions,
        "name", sol::property(&Animation::getName, &Animation::setName),
        "getName", &Animation::getName,
        "setName", &Animation::setName,
        "addActionFrame", sol::overload( sol::resolve<void(float, float, Entity, Entity)>(&Animation::addActionFrame), sol::resolve<void(float, Entity, Entity)>(&Animation::addActionFrame) ),
        "getActionFrame", &Animation::getActionFrame
        );

    lua.new_usertype<Ease>("Ease",
        "linear", Ease::linear,
        "easeInQuad", Ease::easeInQuad
        );

}