#include "Timeline.h"
#include "Engine.h"
#include "Scene.h"

#include <stdio.h>

using namespace Supernova;

Timeline::Timeline(){
    this->function = S_TIMELINE_LINEAR;
    this->duration = 0;
    this->loop = false;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}

Timeline::Timeline(int duration, bool loop){
    this->function = S_TIMELINE_LINEAR;
    this->duration = duration;
    this->loop = loop;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}

Timeline::Timeline(int duration, bool loop, int function){
    this->function = function;
    this->duration = duration;
    this->loop = loop;
    this->started = false;
    this->timecount = 0;
    this->time = 0;
    this->value = 0;
}


Timeline::~Timeline(){
    
}

bool Timeline::isStarted(){
    return started;
}

void Timeline::start(){
    started = true;
}

void Timeline::stop(){
    started = false;
}

void Timeline::step(){
    if (started){
        timecount += Engine::getFrametime();
        if (timecount >= duration){
            if (!loop){
                stop();
                timecount = duration;
            }else{
                timecount -= duration;
            }
        }
    }
    
    time = (float)timecount / duration;
    if (S_TIMELINE_LINEAR){
        value = time;
    }else if (S_TIMELINE_EXPONENTIAL){
        value = time;
    }
    
    printf("step time %f value %f \n", time, value);
}
