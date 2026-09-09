// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef REFLECTION_PROBE_COMPONENT_H
#define REFLECTION_PROBE_COMPONENT_H

#include "math/Vector3.h"
#include "texture/Texture.h"

namespace doriax{

    enum class ReflectionProbeMode{
        STATIC,
        DYNAMIC
    };

    enum class ReflectionProbeUpdateMode{
        ON_LOAD,
        ON_MOVE,
        INTERVAL,
        MANUAL
    };

    // Local, box-shaped specular environment. Static probes use an authored
    // cubemap when supplied, otherwise they capture once at runtime. Dynamic
    // probes capture according to their update mode. The forward renderer picks
    // one probe per renderable (using its world-bounds center), not per fragment.
    // GPU state is owned by RenderSystem rather than serialized here.
    struct DORIAX_API ReflectionProbeComponent{
        ReflectionProbeMode mode = ReflectionProbeMode::STATIC;
        ReflectionProbeUpdateMode updateMode = ReflectionProbeUpdateMode::ON_LOAD;

        // Local-space influence center. Rotation and scale affect this offset;
        // the resulting influence volume remains a world-axis-aligned box.
        // Runtime captures are taken from the entity origin, not this offset.
        Vector3 boxOffset = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 boxSize = Vector3(10.0f, 10.0f, 10.0f);
        float blendDistance = 1.0f;
        float intensity = 1.0f;
        int priority = 0;

        unsigned int resolution = 128;
        float nearClip = 0.1f;
        float farClip = 100.0f;
        float updateInterval = 1.0f;
        bool includeSky = true;

        // Six-face cubemap used by static probes. Dynamic probes ignore it.
        Texture texture;

        // Runtime invalidation only; deliberately not serialized.
        bool needUpdate = true;
        unsigned int captureRevision = 0;
    };

}

#endif // REFLECTION_PROBE_COMPONENT_H
