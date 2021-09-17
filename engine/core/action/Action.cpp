//
// (c) 2021 Eduardo Doria.
//

#include "Action.h"

using namespace Supernova;

Action::Action(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    addComponent<ActionComponent>({});
}

void Action::start(){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);

    action.startTrigger = true;
}

void Action::pause(){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);

    action.pauseTrigger = true;
}

void Action::stop(){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);

    action.stopTrigger = true;
}

void Action::setTarget(Entity target){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);

    action.target = target;
}

Entity Action::getEntity(){
    return entity;
}