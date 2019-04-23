//
// (c) 2019 Eduardo Doria.
//

#include "TranslateTracks.h"
#include "Object.h"
#include "Log.h"

using namespace Supernova;

TranslateTracks::TranslateTracks(): KeyframeTrack(){
}

TranslateTracks::TranslateTracks(std::vector<float> times, std::vector<Vector3> values): KeyframeTrack(times){
    setValues(values);
}

void TranslateTracks::setValues(std::vector<Vector3> values){
    this->values = values;
}

void TranslateTracks::addKeyframe(float time, Vector3 value){
    times.push_back(time);
    values.push_back(value);
}

bool TranslateTracks::update(float interval){
    if (!KeyframeTrack::update(interval))
        return false;

    if (object){
        Vector3 move = (values[index+1] - values[index]) * progress;
        object->setPosition(values[index] + move);
    }

    return true;
}