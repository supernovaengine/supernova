#include "PhysicsWorld.h"

#include "Scene.h"
#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "Log.h"

#include "LuaIntf.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

PhysicsWorld::PhysicsWorld(){
    ownedBodies = true;
    attachedScene = NULL;
}

PhysicsWorld::~PhysicsWorld(){
    if (attachedScene)
        attachedScene->removePhysics(this);

    if (ownedBodies){
        for (int i = 0; i < bodies.size(); i++)
            delete bodies[i];
    }
}

void PhysicsWorld::setOwnedBodies(bool ownedBodies){
    this->ownedBodies = ownedBodies;
}

bool PhysicsWorld::isOwnedBodies(){
    return ownedBodies;
}

void PhysicsWorld::updateBodyObjects(){
    for (int i =0; i < bodies.size(); i++){
        bodies[i]->updateObject();
    }
}