//
// (c) 2023 Eduardo Doria.
//

#include "PhysicsSystem.h"
#include "Scene.h"

#include "box2d.h"


using namespace Supernova;


PhysicsSystem::PhysicsSystem(Scene* scene): SubSystem(scene){
	signature.set(scene->getComponentType<Transform>());

	this->scene = scene;
}

PhysicsSystem::~PhysicsSystem(){
}


void PhysicsSystem::load(){
    b2Vec2 gravity(0.0f, -10.0f);
    b2World world(gravity);
}

void PhysicsSystem::destroy(){

}

void PhysicsSystem::update(double dt){

}

void PhysicsSystem::draw(){

}

void PhysicsSystem::entityDestroyed(Entity entity){

}
