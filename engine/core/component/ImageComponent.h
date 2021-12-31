//
// (c) 2021 Eduardo Doria.
//

#ifndef IMAGE_COMPONENT_H
#define IMAGE_COMPONENT_H


namespace Supernova{

    struct ImageComponent{
        int patchMarginLeft = 0;
        int patchMarginRight = 0;
        int patchMarginTop = 0;
        int patchMarginBottom = 0;

        bool needUpdate = true;
    };

}

#endif //IMAGE_COMPONENT_H