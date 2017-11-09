#include "Action.h"

#include "Engine.h"
#include "Object.h"
#include "platform/Log.h"

using namespace Supernova;

Action::Action(){
    this->object = NULL;
    this->running = false;
    this->timecount = 0;
    this->steptime = 0;
}

Action::~Action(){
    if (object)
        object->removeAction(this);
}

bool Action::isRunning(){
    return running;
}

bool Action::run(){
    running = true;

    return true;
}

bool Action::pause(){
    running = false;
    
    return true;
}

bool Action::stop(){
    running = false;
    timecount = 0;
    
    return true;
}

bool Action::step(){
    if (running){
        steptime = Engine::getDeltatime();
        timecount += steptime;
    }else{
        return false;
    }
    
    return true;
}
