//
// (c) 2024 Eduardo Doria.
//

#ifndef PANEL_H
#define PANEL_H

#include "Image.h"
#include "Text.h"

namespace Supernova{
    class Panel: public Image{

    public:
        Panel(Scene* scene);

        Text getTitleObject() const;

        void setTitle(std::string text);
        std::string getTitle() const;

        void setTitleColor(Vector4 color);
        void setTitleColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getTitleColor() const;

        void setTitleFont(std::string font);
        std::string getTitleFont() const;

        void setTitleFontSize(unsigned int fontSize);
        unsigned int getTitleFontSize() const;
    };
}

#endif //PANEL_H