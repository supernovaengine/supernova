// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SPRITEFRAMEDATA_H
#define SPRITEFRAMEDATA_H

#include "math/Rect.h"

namespace doriax{

    struct SpriteFrameData{
        std::string name;
        Rect rect;
    };

}

#endif //SPRITEFRAMEDATA_H