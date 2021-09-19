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

Entity Action::getEntity(){
    return entity;
}