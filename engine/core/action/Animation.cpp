//
// (c) 2019 Eduardo Doria.
//

#include "Animation.h"
#include "Log.h"

using namespace Supernova;

Animation::Animation(): Action(){
    this->ownedActions = true;
    this->loop = true;
    this->name = "";
    this->startTime = 0;
    this->endTime = FLT_MAX;
}

Animation::Animation(std::string name, bool loop): Action(){
    this->ownedActions = true;
    this->loop = loop;
    this->name = name;
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
        timecount = startTime;
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

Animation::ActionFrame Animation::getActionFrame(unsigned int index){
    if (index >= actions.size()){
        Log::Error("ActionFrame index %i is not created", index);
    }else{
        return actions[index];
    }
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

const std::string &Animation::getName() const {
    return name;
}

void Animation::setName(const std::string &name) {
    Animation::name = name;
}

bool Animation::run(){
    if (!Action::run())
        return false;

    if (timecount == startTime)
        onStart.call(object);

    return true;
}

bool Animation::stop(){
    if (!Action::stop())
        return false;

    for (int i = 0; i < actions.size(); i++){
        actions[i].action->stop();
    }

    timecount = startTime;

    return true;
}

bool Animation::update(float interval){
    if (!Action::update(interval))
        return false;

    int totalActionsPassed = 0;

    for (int i = 0; i < actions.size(); i++){

        float timeDiff = timecount - actions[i].startTime;

        if (timeDiff >= 0) {
            //TODO: Support loop actions
            if (timeDiff <= actions[i].action->getDuration()) {
                if (!actions[i].action->isRunning()) {
                    actions[i].action->run();
                }
                actions[i].action->setTimecount(timeDiff);
                actions[i].action->update(interval);
            }else{
                totalActionsPassed++;
            }
        }

    }

    if (totalActionsPassed == actions.size() || timecount >= endTime) {
        if (!loop) {
            stop();
            onFinish.call(object);
            return false;
        }else{
            timecount = startTime;
        }
    }


    return true;
}