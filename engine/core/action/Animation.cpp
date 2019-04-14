//
// (c) 2019 Eduardo Doria.
//

#include "Animation.h"
#include "Log.h"

using namespace Supernova;

Animation::Animation(): Action(){
    this->ownedActions = true;
    this->loop = true;
    this->startTime = 0;
    this->endTime = FLT_MAX;
}

Animation::Animation(bool loop): Action(){
    this->ownedActions = true;
    this->loop = loop;
    this->startTime = 0;
    this->endTime = FLT_MAX;
}

Animation::~Animation(){
    clearActionFrames();
}

bool Animation::isLoop(){
    return loop;
}

void Animation::setLoop(bool loop){
    this->loop = loop;
}

void Animation::setStartTime(float startTime){
    this->startTime = startTime;
    if (!isRunning())
        timecount = (long)(startTime * 1000);
}

float Animation::getStartTime(){
    return startTime;
}

void Animation::setEndTime(float endTime){
    this->endTime = endTime;
}

float Animation::getEndTime(){
    return endTime;
}

void Animation::setLimits(float startTime, float endTime){
    setStartTime(startTime);
    setEndTime(endTime);
}

void Animation::addActionFrame(float startTime, TimeAction* action, Object* object){
    ActionFrame actionFrame;

    actionFrame.startTime = startTime;
    actionFrame.action = action;
    action->object = object;

    actions.push_back(actionFrame);
}

void Animation::clearActionFrames(){
    if (ownedActions){
        for (int i = 0; i < actions.size(); i++){
            delete actions[i].action;
        }
    }
    actions.clear();
}

bool Animation::isOwnedActions() const {
    return ownedActions;
}

void Animation::setOwnedActions(bool ownedActions) {
    Animation::ownedActions = ownedActions;
}

bool Animation::run(){
    if (!Action::run())
        return false;

    if (timecount == (long)(startTime * 1000))
        onStart.call(object);

    return true;
}

bool Animation::stop(){
    if (!Action::stop())
        return false;

    timecount = (long)(startTime * 1000);

    return true;
}

bool Animation::step(){
    if (!Action::step())
        return false;

    float timesec = timecount / (float)1000;
    int totalActionsPassed = 0;

    for (int i = 0; i < actions.size(); i++){

        float timeDiff = timesec - actions[i].startTime;

        if (timeDiff >= 0) {
            if (timeDiff <= actions[i].action->getDuration()) {
                if (!actions[i].action->isRunning()) {
                    actions[i].action->setTimecount((int) (timeDiff * 1000));
                    actions[i].action->run();
                }
            }else{
                totalActionsPassed++;
            }
        }

        actions[i].action->step();

    }

    if (totalActionsPassed == actions.size() || timesec >= endTime) {
        if (!loop) {
            stop();
            onFinish.call(object);
            return false;
        }else{
            timecount = (long)(startTime * 1000);
        }
    }


    return true;
}
