//
// (c) 2019 Eduardo Doria.
//

#include "MorphTracks.h"

#include "Model.h"
#include "Log.h"

using namespace Supernova;

MorphTracks::MorphTracks(){
}

MorphTracks::MorphTracks(std::vector<float> times, std::vector<std::vector<float>> values){
    setValues(values);
}

void MorphTracks::setValues(std::vector<std::vector<float>> values){
    this->values = values;
}

void MorphTracks::addKeyframe(float time, std::vector<float> value){
    times.push_back(time);
    values.push_back(value);
}

bool MorphTracks::update(float interval){
    if (!KeyframeTrack::update(interval))
        return false;

    if (Model* model = dynamic_cast<Model*>(object)){
        if (values[index].size() == values[index+1].size()) {
            for (int morphIndex = 0; morphIndex < values[index].size(); morphIndex++) {
                float weight = (values[index + 1][morphIndex] - values[index][morphIndex]) * progress;
                model->setMorphWeight(morphIndex, values[index][morphIndex] + weight);
            }
        }else{
            Log::Error("MorphTrack of index %i is different size than index %i", index, index+1);
        }
    }

    return true;
}
