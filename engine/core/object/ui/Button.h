//
// (c) 2022 Eduardo Doria.
//

#ifndef BUTTON_H
#define BUTTON_H

#include "Image.h"

namespace Supernova{
    class Button: public Image{

    public:
        Button(Scene* scene);

        void setLabel(std::string text);
        std::string getLabel();

        void setLabelColor(Vector4 color);
        void setLabelColor(float red, float green, float blue, float alpha);
        Vector4 getLabelColor();

        void setTextureNormal(std::string path);
        void setTexturePressed(std::string path);
        void setTextureDisabled(std::string path);
    };
}

#endif //BUTTON_H