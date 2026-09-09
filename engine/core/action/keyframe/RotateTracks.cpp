// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "RotateTracks.h"

#include "component/KeyframeTracksComponent.h"
#include "component/RotateTracksComponent.h"

using namespace doriax;

RotateTracks::RotateTracks(Scene* scene): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<RotateTracksComponent>();
}

RotateTracks::RotateTracks(Scene* scene, Entity entity): Action(scene, entity){

}

RotateTracks::RotateTracks(Scene* scene, std::vector<float> times, std::vector<Quaternion> values): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<RotateTracksComponent>();

    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    RotateTracksComponent& rotatetracks = getComponent<RotateTracksComponent>();

    rotatetracks.values = values;
}

void RotateTracks::setTimes(std::vector<float> times){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    // keep per-segment easings aligned (trailing entries would go stale)
    size_t segments = keyframe.times.size() > 1 ? keyframe.times.size() - 1 : 0;
    if (keyframe.easings.size() > segments){
        keyframe.easings.resize(segments);
    }
}

void RotateTracks::setValues(std::vector<Quaternion> values){
    RotateTracksComponent& rotatetracks = getComponent<RotateTracksComponent>();

    rotatetracks.values = values;

    // keep Hermite tangents (GLTF CUBICSPLINE) mirrored with values
    if (!rotatetracks.inTangents.empty()){
        rotatetracks.inTangents.resize(values.size(), Quaternion(0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (!rotatetracks.outTangents.empty()){
        rotatetracks.outTangents.resize(values.size(), Quaternion(0.0f, 0.0f, 0.0f, 0.0f));
    }
}

void RotateTracks::setEasings(std::vector<EaseType> easings){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    // CUSTOM has no per-segment function storage; treat it as linear
    for (EaseType& e : easings){
        if (e == EaseType::CUSTOM) e = EaseType::LINEAR;
    }

    keyframe.easings = easings;
}

void RotateTracks::setEasing(unsigned int segment, EaseType ease){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    // CUSTOM has no per-segment function storage; treat it as linear
    if (ease == EaseType::CUSTOM){
        ease = EaseType::LINEAR;
    }

    if (keyframe.easings.size() <= segment){
        keyframe.easings.resize(segment + 1, EaseType::LINEAR);
    }
    keyframe.easings[segment] = ease;
}
