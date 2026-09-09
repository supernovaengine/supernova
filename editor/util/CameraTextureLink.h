// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Scene.h"
#include "texture/Texture.h"

namespace doriax::editor {

    // A texture can use a camera framebuffer as source (render to texture).
    // The link is kept in the texture id as "camera|<entity>" so it survives
    // serialization; the framebuffer pointer is (re)bound by resolve().
    class CameraTextureLink {
    public:
        static std::string makeId(Entity cameraEntity);
        static Entity parse(const std::string& textureId);
        static bool isCameraTexture(const Texture& texture);

        static Texture make(EntityRegistry* registry, Entity cameraEntity);

        // display name for the texture slot ("camera|7" -> camera entity name)
        static std::string cameraName(EntityRegistry* registry, const Texture& texture);

        // true if any texture in the registry links to this camera entity
        static bool isCameraUsed(EntityRegistry* registry, Entity cameraEntity);

        // rebind every camera-linked texture in the registry to the current
        // framebuffer of its camera entity (or unbind if the camera is gone),
        // flagging affected components to reload; returns true if any changed
        static bool resolve(EntityRegistry* registry);
    };

}
