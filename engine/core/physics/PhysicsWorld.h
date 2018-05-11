
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "Body.h"

typedef struct lua_State lua_State;

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class PhysicsWorld {
    private:
        void (*onBeginContactFunc)(Body*, Body*);
        int onBeginContactLuaFunc;

        void (*onEndContactFunc)(Body*, Body*);
        int onEndContactLuaFunc;

    public:
        PhysicsWorld();

        void call_onBeginContact(Body* bodyA, Body* bodyB);
        void call_onEndContact(Body* bodyA, Body* bodyB);

        void onBeginContact(void (*onBeginContactFunc)(Body*, Body*));
        int onBeginContact(lua_State *L);

        void onEndContact(void (*onEndContactFunc)(Body*, Body*));
        int onEndContact(lua_State *L);

        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
