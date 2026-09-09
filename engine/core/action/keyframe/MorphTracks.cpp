// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "MorphTracks.h"

#include "component/KeyframeTracksComponent.h"
#include "component/MorphTracksComponent.h"

using namespace doriax;

MorphTracks::MorphTracks(Scene* scene): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<MorphTracksComponent>();
}

MorphTracks::MorphTracks(Scene* scene, Entity entity): Action(scene, entity){

}

MorphTracks::MorphTracks(Scene* scene, std::vector<float> times, std::vector<std::vector<float>> values): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<MorphTracksComponent>();

    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    MorphTracksComponent& morphtracks = getComponent<MorphTracksComponent>();

    morphtracks.values = values;
}

void MorphTracks::setTimes(std::vector<float> times){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    // keep per-segment easings aligned (trailing entries would go stale)
    size_t segments = keyframe.times.size() > 1 ? keyframe.times.size() - 1 : 0;
    if (keyframe.easings.size() > segments){
        keyframe.easings.resize(segments);
    }
}

void MorphTracks::setValues(std::vector<std::vector<float>> values){
    MorphTracksComponent& morphtracks = getComponent<MorphTracksComponent>();

    morphtracks.values = values;

    // keep Hermite tangents (GLTF CUBICSPLINE) mirrored with values
    std::vector<float> zeroTangent(values.empty() ? 0 : values.back().size(), 0.0f);
    if (!morphtracks.inTangents.empty()){
        morphtracks.inTangents.resize(values.size(), zeroTangent);
    }
    if (!morphtracks.outTangents.empty()){
        morphtracks.outTangents.resize(values.size(), zeroTangent);
    }
}

void MorphTracks::setEasings(std::vector<EaseType> easings){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    // CUSTOM has no per-segment function storage; treat it as linear
    for (EaseType& e : easings){
        if (e == EaseType::CUSTOM) e = EaseType::LINEAR;
    }

    keyframe.easings = easings;
}

void MorphTracks::setEasing(unsigned int segment, EaseType ease){
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
