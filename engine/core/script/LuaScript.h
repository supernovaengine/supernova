//
// (c) 2020 Eduardo Doria.
//

#ifndef LUA_H
#define LUA_H

#include "Object.h"

namespace Supernova {
    class LuaScript {
    public:
        static void setObject(const char* global, Object* object);
        static Object* getObject(const char* global);
    };

}


#endif //ANDROIDSTUDIO_LUA_H
