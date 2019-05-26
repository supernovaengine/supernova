//
// (c) 2019 Eduardo Doria.
//

#include "MorphAction.h"

#include "Model.h"

using namespace Supernova;

MorphAction::MorphAction(int morphIndex, float duration, bool loop): TimeAction(duration, loop){
    this->morphIndex = morphIndex;
    this->objectStartWeight = true;
}

MorphAction::MorphAction(int morphIndex, float endWeight, float duration, bool loop): TimeAction(duration, loop){
    this->morphIndex = morphIndex;
    this->endWeight = endWeight;
    this->objectStartWeight = true;
}

MorphAction::MorphAction(int morphIndex, float startWeight, float endWeight, float duration, bool loop): TimeAction(duration, loop){
    this->morphIndex = morphIndex;
    this->startWeight = startWeight;
    this->endWeight = endWeight;
    this->objectStartWeight = false;
}

MorphAction::~MorphAction(){

}

void MorphAction::setMorphIndex(int morphIndex){
    this->morphIndex = morphIndex;
}

void MorphAction::setEndWeight(float endWeight){
    this->endWeight = endWeight;
}

void MorphAction::setStartWeight(float startWeight){
    this->startWeight = startWeight;
    this->objectStartWeight = false;
}

bool MorphAction::run(){
    if (!TimeAction::run())
        return false;

    if (Model* model = dynamic_cast<Model*>(object)) {
        if (objectStartWeight) {
            startWeight = model->getMorphWeight(morphIndex);
        }
    }

    return true;
}

bool MorphAction::update(float interval){
    if (!TimeAction::update(interval))
        return false;

    if (Model* model = dynamic_cast<Model*>(object)){
        float weight = (endWeight - startWeight) * value;
        model->setMorphWeight(morphIndex, startWeight + weight);
    }

    return true;
}
