//
// (c) 2019 Eduardo Doria.
//

#include "KeyframeTrack.h"
#include "Log.h"

using namespace Supernova;

KeyframeTrack::KeyframeTrack(): TimeAction(){
    this->index = 0;
    this->progress = 0;
}

KeyframeTrack::KeyframeTrack(std::vector<float> times): TimeAction(){
    setTimes(times);
    this->index = 0;
    this->progress = 0;
}

void KeyframeTrack::setTimes(std::vector<float> times){
    this->times = times;
}

std::vector<float> KeyframeTrack::getTimes(){
    return times;
}

bool KeyframeTrack::stop(){
    if (!Action::stop())
        return false;

    index = 0;
    progress = 0;

    return true;
}

bool KeyframeTrack::update(float interval){
    if (!TimeAction::update(interval))
        return false;

    if (times.size() == 0)
        return false;

    float actualTime = duration * value;

    if (actualTime < times[0]) {
        index = 0;
        progress = 0;
        return true;
    }

    index = 0;
    while ((index+1) < times.size() && times[index+1] < actualTime){
        index++;
    }

    return true;
}
