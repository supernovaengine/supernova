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
    
    time--;
    return - 0.5 * (time * (time - 2) - 1);
}

float Action::cubicEaseIn(float time){
    return time * time * time;
}

float Action::cubicEaseOut(float time){
    time--;
    return time * time * time + 1;
}

float Action::cubicEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time;
    }
    time -= 2;
    return 0.5 * (time * time * time + 2);
}

float Action::quartEaseIn(float time){
    return time * time * time * time;
}

float Action::quartEaseOut(float time){
    time--;
    return 1 - (time * time * time * time);
}

float Action::quartEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time;
    }
    time -= 2;
    return - 0.5 * (time * time * time * time - 2);
}

float Action::quintEaseIn(float time){
    return time * time * time * time * time;
}

float Action::quintEaseOut(float time){
    time--;
    return time * time * time * time * time + 1;
}

float Action::quintEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time * time;
    }
    
    time -= 2;
    return 0.5 * (time * time * time * time * time + 2);
}

float Action::sineEaseIn(float time){
    return 1 - cos(time * M_PI / 2);
}

float Action::sineEaseOut(float time){
    return sin(time * M_PI / 2);
}

float Action::sineEaseInOut(float time){
    return 0.5 * (1 - cos(M_PI * time));
}

float Action::expoEaseIn(float time){
    return time == 0 ? 0 : pow(1024, time - 1);
}

float Action::expoEaseOut(float time){
    return time == 1 ? 1 : 1 - pow(2, - 10 * time);
}

float Action::expoEaseInOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    if ((time *= 2) < 1) {
        return 0.5 * pow(1024, time - 1);
    }
    
    return 0.5 * (- pow(2, - 10 * (time - 1)) + 2);
}

float Action::circEaseIn(float time){
    return 1 - sqrt(1 - time * time);
}

float Action::circEaseOut(float time){
    time--;
    return sqrt(1 - (time * time));
}

float Action::circEaseInOut(float time){
    if ((time *= 2) < 1) {
        return - 0.5 * (sqrt(1 - time * time) - 1);
    }
    
    time -= 2;
    return 0.5 * (sqrt(1 - time * time) + 1);
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

float Action::backEaseIn(float time){
    float s = 1.70158;
    
    return time * time * ((s + 1) * time - s);
}

float Action::backEaseOut(float time){
    float s = 1.70158;
    
    time--;
    return time * time * ((s + 1) * time + s) + 1;
}

float Action::backEaseInOut(float time){
    float s = 1.70158 * 1.525;
    
    if ((time *= 2) < 1) {
        return 0.5 * (time * time * ((s + 1) * time - s));
    }
    
    time -= 2;
    return 0.5 * (time * time * ((s + 1) * time + s) + 2);
}

float Action::bounceEaseIn(float time){
    return 1 - bounceEaseOut(1 - time);
}

float Action::bounceEaseOut(float time){
    if (time < (1 / 2.75)) {
        return 7.5625 * time * time;
    } else if (time < (2 / 2.75)) {
        time -= (1.5 / 2.75);
        return 7.5625 * time * time + 0.75;
    } else if (time < (2.5 / 2.75)) {
        time -= (2.25 / 2.75);
        return 7.5625 * time * time + 0.9375;
    } else {
        time -= (2.625 / 2.75);
        return 7.5625 * time * time + 0.984375;
    }
}

float Action::bounceEaseInOut(float time){
    if (time < 0.5) {
        return bounceEaseIn(time * 2) * 0.5;
    }
    
    return bounceEaseOut(time * 2 - 1) * 0.5 + 0.5;
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
    if (function)
        value = function(time);
    
    Log::Debug(LOG_TAG, "step time %f value %f \n", time, value);
}
