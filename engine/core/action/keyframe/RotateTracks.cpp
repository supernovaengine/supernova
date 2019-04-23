//
// (c) 2019 Eduardo Doria.
//

#include "RotateTracks.h"
#include "Object.h"

using namespace Supernova;


RotateTracks::RotateTracks(){
}

RotateTracks::RotateTracks(std::vector<float> times, std::vector<Quaternion> values){
    setValues(values);
}

void RotateTracks::setValues(std::vector<Quaternion> values){
    this->values = values;
}

void RotateTracks::addKeyframe(float time, Quaternion value){
    times.push_back(time);
    values.push_back(value);
}

bool RotateTracks::update(float interval){
    if (!KeyframeTrack::update(interval))
        return false;

    if (object){
        object->setRotation(object->getRotation().slerp(progress, values[index], values[index+1]));
    }

    return true;
}