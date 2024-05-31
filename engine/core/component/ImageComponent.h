//
// (c) 2024 Eduardo Doria.
//

#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H

namespace Supernova{

    struct ImageComponent{
        //Nine patch rect
        int patchMarginLeft = 0;
        int patchMarginRight = 0;
        int patchMarginTop = 0;
        int patchMarginBottom = 0;

        float textureCutFactor = 0.0;

        bool needUpdatePatches = true;
    };

}

#endif //IMAGE_COMPONENT_H