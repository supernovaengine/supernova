// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ScaleTracks.h"

#include "component/KeyframeTracksComponent.h"
#include "component/ScaleTracksComponent.h"

using namespace doriax;

ScaleTracks::ScaleTracks(Scene* scene): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<ScaleTracksComponent>();
}

ScaleTracks::ScaleTracks(Scene* scene, Entity entity): Action(scene, entity){

}

ScaleTracks::ScaleTracks(Scene* scene, std::vector<float> times, std::vector<Vector3> values): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<ScaleTracksComponent>();

    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    ScaleTracksComponent& scaletracks = getComponent<ScaleTracksComponent>();

    scaletracks.values = values;
}

void ScaleTracks::setTimes(std::vector<float> times){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    // keep per-segment easings aligned (trailing entries would go stale)
    size_t segments = keyframe.times.size() > 1 ? keyframe.times.size() - 1 : 0;
    if (keyframe.easings.size() > segments){
        keyframe.easings.resize(segments);
    }
}

void ScaleTracks::setValues(std::vector<Vector3> values){
    ScaleTracksComponent& scaletracks = getComponent<ScaleTracksComponent>();

    scaletracks.values = values;

    // keep Hermite tangents (GLTF CUBICSPLINE) mirrored with values
    if (!scaletracks.inTangents.empty()){
        scaletracks.inTangents.resize(values.size(), Vector3::ZERO);
    }
    if (!scaletracks.outTangents.empty()){
        scaletracks.outTangents.resize(values.size(), Vector3::ZERO);
    }
}

void ScaleTracks::setEasings(std::vector<EaseType> easings){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    // CUSTOM has no per-segment function storage; treat it as linear
    for (EaseType& e : easings){
        if (e == EaseType::CUSTOM) e = EaseType::LINEAR;
    }

    keyframe.easings = easings;
}

void ScaleTracks::setEasing(unsigned int segment, EaseType ease){
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
