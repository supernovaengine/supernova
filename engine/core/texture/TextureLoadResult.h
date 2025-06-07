//
// (c) 2025 Eduardo Doria.
//

#ifndef TEXTURELOADRESULT_H
#define TEXTURELOADRESULT_H

#include <string>
#include <memory>
#include <array>
#include "Engine.h"
#include "TextureData.h"

namespace Supernova {

    class TextureLoadResult {
    public:
        std::string id;
        ResourceLoadState state = ResourceLoadState::NotStarted;
        std::string errorMessage;
        std::shared_ptr<std::array<TextureData, 6>> data = nullptr;

        TextureLoadResult() = default;

        explicit operator bool() const {
            return state == ResourceLoadState::Ready;
        }
    };

}

#endif // TEXTURELOADRESULT_H