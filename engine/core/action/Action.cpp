#include "Action.h"

#include "Engine.h"
#include "Object.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

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

Object* Action::getObject(){
    return object;
}

void Action::setTimecount(unsigned long timecount){
    this->timecount = timecount;
}

bool Action::isRunning(){
    return running;
}

bool Action::run(){
    running = true;
    if (timecount == 0)
        onStart.call(object);
    onRun.call(object);

    return true;
}

bool Action::pause(){
    running = false;
    onPause.call(object);
    
    return true;
}

bool Action::stop(){
    running = false;
    timecount = 0;
    onStop.call(object);
    
    return true;
}

bool Action::step(){
    onStep.call(object);
    
    return true;
}

bool Action::update(float time){
    if (running){
        steptime = time * 1000;
        timecount += steptime;
        step();
    }else{
        return false;
    }

    return true;
}
