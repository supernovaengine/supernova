//
// (c) 2019 Eduardo Doria.
//

#include "ScaleTracks.h"
#include "Object.h"
#include "Log.h"

using namespace Supernova;

ScaleTracks::ScaleTracks(): KeyframeTrack(){
}

ScaleTracks::ScaleTracks(std::vector<float> times, std::vector<Vector3> values): KeyframeTrack(times){
    setValues(values);
}

void ScaleTracks::setValues(std::vector<Vector3> values){
    this->values = values;
}

void ScaleTracks::addKeyframe(float time, Vector3 value){
    times.push_back(time);
    values.push_back(value);
}

bool ScaleTracks::update(float interval){
    if (!KeyframeTrack::update(interval))
        return false;

    if (object){
        Vector3 scale = (values[index+1] - values[index]) * progress;
        object->setScale(values[index] + scale);
    }

    return true;
}
