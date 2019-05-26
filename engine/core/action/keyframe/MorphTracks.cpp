//
// (c) 2019 Eduardo Doria.
//

#include "MorphTracks.h"

#include "Model.h"

using namespace Supernova;

MorphTracks::MorphTracks(){
    this->morphIndex = 0;
}

MorphTracks::MorphTracks(int morphIndex, std::vector<float> times, std::vector<float> values){
    setValues(values);
}

void MorphTracks::setMorphIndex(int morphIndex){
    this->morphIndex = morphIndex;
}

void MorphTracks::setValues(std::vector<float> values){
    this->values = values;
}

void MorphTracks::addKeyframe(float time, float value){
    times.push_back(time);
    values.push_back(value);
}

bool MorphTracks::update(float interval){
    if (!KeyframeTrack::update(interval))
        return false;

    if (Model* model = dynamic_cast<Model*>(object)){
        float weight = (values[index+1] - values[index]) * progress;
        model->setMorphWeight(morphIndex, values[index] + weight);
    }

    return true;
}
