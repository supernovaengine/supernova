
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "CollisionShape.h"

typedef struct lua_State lua_State;

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class PhysicsWorld {
    private:
        void (*onBeginContactFunc)(CollisionShape*, CollisionShape*);
        int onBeginContactLuaFunc;

        void (*onEndContactFunc)(CollisionShape*, CollisionShape*);
        int onEndContactLuaFunc;

    protected:
        PhysicsWorld();

    public:
        void call_onBeginContact(CollisionShape* shapeA, CollisionShape* shapeB);
        void call_onEndContact(CollisionShape* shapeA, CollisionShape* shapeB);

        void onBeginContact(void (*onBeginContactFunc)(CollisionShape*, CollisionShape*));
        int onBeginContact(lua_State *L);

        void onEndContact(void (*onEndContactFunc)(CollisionShape*, CollisionShape*));
        int onEndContact(lua_State *L);

        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
