#include "TimeAction.h"

#include "Engine.h"
#include "Object.h"
#include "Log.h"
#include <math.h>

#include <stdio.h>

using namespace Supernova;


TimeAction::TimeAction(): Action(), Ease(){
    this->duration = 0;
    this->loop = false;
    this->time = 0;
    this->value = 0;
}

TimeAction::TimeAction(float duration, bool loop): Action(), Ease(){
    this->duration = duration;
    this->loop = loop;
    this->time = 0;
    this->value = 0;
}

TimeAction::TimeAction(float duration, bool loop, float (*function)(float)): Action(), Ease(){
    this->function = function;

    this->duration = duration;
    this->loop = loop;
    this->time = 0;
    this->value = 0;
}


TimeAction::~TimeAction(){

}

float TimeAction::getDuration(){
    return duration;
};

void TimeAction::setDuration(float duration){
     this->duration = duration;
}

bool TimeAction::isLoop(){
    return loop;
}

void TimeAction::setLoop(bool loop){
    this->loop = loop;
}

float TimeAction::getTime(){
    return time;
}

float TimeAction::getValue(){
    return value;
}

bool TimeAction::run(){
    return Action::run();
}

bool TimeAction::pause(){
    return Action::pause();
}

bool TimeAction::stop(){
    if (!Action::stop())
        return false;

    time = 0;
    value = 0;

    return true;
}

bool TimeAction::update(float interval){
    if (!Action::update(interval))
        return false;
    
    if ((time == 1) && !loop){
        stop();
        onFinish.call(object);
        return false;
    }
    
    if (duration >= 0) {
        
        int durationms = (int)(duration * 1000);
        
        if (timecount >= durationms){
            if (!loop){
                timecount = durationms;
            }else{
                timecount -= durationms;
            }
        }
        
        time = (float) timecount / durationms;
    }

    value = function.call(time);
    //Log::Debug("step time %f value %f \n", time, value);
    
    return true;
}
