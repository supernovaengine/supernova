//
// (c) 2022 Eduardo Doria.
//

#ifndef BUTTON_H
#define BUTTON_H

#include "Image.h"
#include "Text.h"

namespace Supernova{
    class Button: public Image{

    public:
        Button(Scene* scene);

        Text getLabelObject() const;

        void setLabel(std::string text);
        std::string getLabel() const;

        void setLabelColor(Vector4 color);
        void setLabelColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getLabelColor() const;

        void setLabelFont(std::string font);
        std::string getLabelFont() const;

        void setLabelFontSize(unsigned int fontSize);
        unsigned int getLabelFontSize() const;

        void setTextureNormal(std::string path);
        void setTexturePressed(std::string path);
        void setTextureDisabled(std::string path);

        void setDisabled(bool disabled);
        bool getDisabled() const;
    };
}

#endif //BUTTON_H