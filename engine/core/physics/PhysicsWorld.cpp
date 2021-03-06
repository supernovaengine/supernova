#include "PhysicsWorld.h"

#include "Scene.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

PhysicsWorld::PhysicsWorld(){
    ownedBodies = true;
    attachedScene = NULL;

    worldSpace = false;
}

PhysicsWorld::~PhysicsWorld(){
    if (attachedScene)
        attachedScene->setPhysicsWorld(NULL);

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

bool PhysicsWorld::addBody(Body* body){
    bool found = false;

    std::vector<Body*>::iterator it;
    for (it = bodies.begin(); it != bodies.end(); ++it) {
        if (body == (*it))
            found = true;
    }

    if (!found){
        bodies.push_back(body);

        return true;
    }

    return false;
}

void PhysicsWorld::removeBody(Body* body){
    std::vector<Body*>::iterator i = std::remove(bodies.begin(), bodies.end(), body);
    bodies.erase(i, bodies.end());
}

void PhysicsWorld::updateBodyObjects(){
    for (int i =0; i < bodies.size(); i++){
        bodies[i]->updateObject();
    }
}

bool PhysicsWorld::isWorldSpace() const {
    return worldSpace;
}

void PhysicsWorld::setWorldSpace(bool worldSpace) {
    PhysicsWorld::worldSpace = worldSpace;
}
