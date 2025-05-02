//
// (c) 2024 Eduardo Doria.
//

#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H

namespace Supernova{

    struct SUPERNOVA_API ImageComponent{
        //Nine patch rect
        unsigned int patchMarginLeft = 0;
        unsigned int patchMarginRight = 0;
        unsigned int patchMarginTop = 0;
        unsigned int patchMarginBottom = 0;

        float textureCutFactor = 0.0;

        bool needUpdatePatches = true;
    };

}

#endif //IMAGE_COMPONENT_H