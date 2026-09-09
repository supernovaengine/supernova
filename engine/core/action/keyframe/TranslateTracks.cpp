// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "TranslateTracks.h"

#include "component/KeyframeTracksComponent.h"
#include "component/TranslateTracksComponent.h"

using namespace doriax;

TranslateTracks::TranslateTracks(Scene* scene): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<TranslateTracksComponent>();
}

TranslateTracks::TranslateTracks(Scene* scene, Entity entity): Action(scene, entity){

}

TranslateTracks::TranslateTracks(Scene* scene, std::vector<float> times, std::vector<Vector3> values): Action(scene){
    addComponent<KeyframeTracksComponent>();
    addComponent<TranslateTracksComponent>();

    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    TranslateTracksComponent& translatetracks = getComponent<TranslateTracksComponent>();

    translatetracks.values = values;
}

void TranslateTracks::setTimes(std::vector<float> times){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    keyframe.times = times;

    // keep per-segment easings aligned (trailing entries would go stale)
    size_t segments = keyframe.times.size() > 1 ? keyframe.times.size() - 1 : 0;
    if (keyframe.easings.size() > segments){
        keyframe.easings.resize(segments);
    }
}

void TranslateTracks::setValues(std::vector<Vector3> values){
    TranslateTracksComponent& translatetracks = getComponent<TranslateTracksComponent>();

    translatetracks.values = values;

    // keep Hermite tangents (GLTF CUBICSPLINE) mirrored with values
    if (!translatetracks.inTangents.empty()){
        translatetracks.inTangents.resize(values.size(), Vector3::ZERO);
    }
    if (!translatetracks.outTangents.empty()){
        translatetracks.outTangents.resize(values.size(), Vector3::ZERO);
    }
}

void TranslateTracks::setEasings(std::vector<EaseType> easings){
    KeyframeTracksComponent& keyframe = getComponent<KeyframeTracksComponent>();

    // CUSTOM has no per-segment function storage; treat it as linear
    for (EaseType& e : easings){
        if (e == EaseType::CUSTOM) e = EaseType::LINEAR;
    }

    keyframe.easings = easings;
}

void TranslateTracks::setEasing(unsigned int segment, EaseType ease){
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
