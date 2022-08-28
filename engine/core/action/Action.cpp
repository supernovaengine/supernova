//
// (c) 2021 Eduardo Doria.
//

#include "Action.h"

using namespace Supernova;

Action::Action(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    entityOwned = true;

    addComponent<ActionComponent>({});
}

Action::Action(Scene* scene, Entity entity){
    this->scene = scene;
    this->entity = entity;
    this->entityOwned = false;
}

Action::~Action(){
    if (scene && entityOwned)
        scene->destroyEntity(entity);
}

Action::Action(const Action& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;
}

Action& Action::operator=(const Action& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;

    return *this;
}

void Action::start(){
    ActionComponent& action = getComponent<ActionComponent>();

    action.startTrigger = true;
}

void Action::pause(){
    ActionComponent& action = getComponent<ActionComponent>();

    action.pauseTrigger = true;
}

void Action::stop(){
    ActionComponent& action = getComponent<ActionComponent>();

    action.stopTrigger = true;
}

void Action::setTarget(Entity target){
    ActionComponent& action = getComponent<ActionComponent>();

    action.target = target;
}

Entity Action::getTarget(){
    ActionComponent& action = getComponent<ActionComponent>();

    return action.target;
}

void Action::setSpeed(float speed){
    ActionComponent& action = getComponent<ActionComponent>();

    action.speed = speed;
}

float Action::getSpeed(){
    ActionComponent& action = getComponent<ActionComponent>();

    return action.speed;
}

bool Action::isRunning(){
    ActionComponent& action = getComponent<ActionComponent>();

    return (action.state == ActionState::Running);
}

Entity Action::getEntity(){
    return entity;
}