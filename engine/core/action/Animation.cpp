//
// (c) 2019 Eduardo Doria.
//

#include "Animation.h"
#include <float.h>
#include "Log.h"
#include "keyframe/KeyframeTrack.h"

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

bool Animation::checkAllKeyframe(){
    int previousFramesSize = -1;

    for (int i = 0; i < actions.size(); i++){
        KeyframeTrack* keyframeAction = dynamic_cast<KeyframeTrack*>(actions[i].action);

        if (keyframeAction){
            int framesSize = keyframeAction->getTimes().size();

            if (previousFramesSize != -1 && previousFramesSize != framesSize){
                Log::Error("The animation (%s) has different sizes of keyframes tracks", name.c_str());
                return false;
            }else{
                previousFramesSize = framesSize;
            }
        }else{
            Log::Error("The animation (%s) is not composed by only keyframe tracks", name.c_str());
            return false;
        }
    }

    return true;
}

void Animation::setStartFrame(int frameIndex){
    if (checkAllKeyframe()){
        std::vector<float> times = ((KeyframeTrack*)actions[0].action)->getTimes();

        if (frameIndex <= 0 || frameIndex > (times.size()-1)){
            Log::Error("Frameindex is out of bound");
        }else{
            setStartTime(times[frameIndex]);
        }
    }
}

void Animation::setStartTime(float startTime){
    this->startTime = startTime;
    if (!isRunning())
        timecount = startTime;
}

float Animation::getStartTime(){
    return startTime;
}


void Animation::setEndFrame(int frameIndex){
    if (checkAllKeyframe()){
        std::vector<float> times = ((KeyframeTrack*)actions[0].action)->getTimes();

        if (frameIndex <= 0 || frameIndex > (times.size()-1)){
            Log::Error("Frameindex is out of bound");
        }else{
            setEndTime(times[frameIndex]);
        }
    }
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

void Animation::addActionFrame(float startTime, float endTime, Action* action, Object* object){
    ActionFrame actionFrame;

    actionFrame.startTime = startTime;
    actionFrame.endTime = endTime;
    actionFrame.action = action;
    action->object = object;

    actions.push_back(actionFrame);
}

void Animation::addActionFrame(float startTime, TimeAction* action, Object* object){
    addActionFrame(startTime, startTime + action->getDuration(), action, object);
}

Animation::ActionFrame Animation::getActionFrame(unsigned int index){
    if (index >= actions.size()){
        Log::Error("ActionFrame index %i is not created", index);
        return actions[0];
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
        float duration = actions[i].endTime - actions[i].startTime;

        if (timeDiff >= 0) {
            //TODO: Support loop actions
            if (timeDiff <= duration) {
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