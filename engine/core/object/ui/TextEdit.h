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


        void setText(std::string text);
        std::string getText();

        void setTextColor(Vector4 color);
        void setTextColor(float red, float green, float blue, float alpha);
        Vector4 getTextColor();
    };
}

#endif //TEXTEDIT_H