#include "Action.h"

#include "Engine.h"
#include "Object.h"
#include "platform/Log.h"
#include <math.h>

#include <stdio.h>

using namespace Supernova;


float Action::linear(float time){
    return time;
}

float Action::quadEaseIn(float time){
    return time * time;
}

float Action::quadEaseOut(float time){
    return time * (2 - time);
}

float Action::quadEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time;
    }
    
    return - 0.5 * ((time - 1) * (time - 3) - 1);
}

float Action::elasticEaseIn(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return -pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
}

float Action::elasticEaseOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return pow(2, -10 * time) * sin((time - 0.1) * 5 * M_PI) + 1;
}

float Action::elasticEaseInOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    time *= 2;
    
    if (time < 1) {
        return -0.5 * pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
    }
    
    return 0.5 * pow(2, -10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI) + 1;
}


Action::Action(){
    this->object = NULL;
    this->function = Action::linear;
    this->duration = 0;
    this->loop = false;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}

Action::Action(float duration, bool loop){
    this->object = NULL;
    this->function = Action::linear;
    this->duration = duration;
    this->loop = loop;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}

Action::Action(float duration, bool loop, float (*function)(float)){
    this->object = NULL;
    this->function = function;
    this->duration = duration;
    this->loop = loop;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}


Action::~Action(){
    if (object)
        object->removeAction(this);
}

void Action::setFunction(float (*function)(float)){
    this->function = function;
}

bool Action::isStarted(){
    return started;
}

void Action::start(){
    started = true;
    time = 0;
    value = 0;
    timecount = 0;
}

void Action::stop(){
    started = false;
}

void Action::step(){
    int durationms = (int)(duration * 1000);

    if (started){
        timecount += Engine::getFrametime();
        if (timecount >= durationms){
            if (!loop){
                stop();
                timecount = durationms;
            }else{
                timecount -= durationms;
            }
        }
    }
    
    time = (float)timecount / durationms;
    value = function(time);
    
    Log::Debug(LOG_TAG, "step time %f value %f \n", time, value);
}
