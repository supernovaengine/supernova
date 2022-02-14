//
// (c) 2022 Eduardo Doria.
//

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "Image.h"

namespace Supernova{
    class TextEdit: public Image{

    public:
        TextEdit(Scene* scene);

        void setDisabled(bool disabled);
    };
}

#endif //TEXTEDIT_H