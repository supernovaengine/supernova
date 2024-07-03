//
// (c) 2024 Eduardo Doria.
//

#ifndef PANEL_H
#define PANEL_H

#include "Image.h"
#include "Text.h"
#include "Container.h"

namespace Supernova{
    class Panel: public Image{

    public:
        Panel(Scene* scene);

        Image getHeaderImageObject() const;
        Container getHeaderContainerObject() const;
        Text getHeaderTextObject() const;

        void setTitle(std::string text);
        std::string getTitle() const;

        void setTitleAnchorPreset(AnchorPreset titleAnchorPreset);
        AnchorPreset getTitleAnchorPreset() const;

        void setTitleColor(Vector4 color);
        void setTitleColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getTitleColor() const;

        void setTitleFont(std::string font);
        std::string getTitleFont() const;

        void setTitleFontSize(unsigned int fontSize);
        unsigned int getTitleFontSize() const;

        void setHeaderColor(Vector4 color);
        void setHeaderColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getHeaderColor() const;

        void setHeaderPatchMargin(int margin);
        void setHeaderTexture(std::string path);

        void setMinSize(int minWidth, int minHeight);
        void setMinWidth(int minWidth);
        void setMinHeight(int minHeight);

        int getMinWidth() const;
        int getMinHeight() const;

        void setWindowProperties(const bool canMove, const bool canResize, const bool canTopOnFocus);

        void setCanMove(const bool canMove);
        bool isCanMove() const;

        void setCanResize(const bool canResize);
        bool isCanResize() const;

        void setCanTopOnFocus(const bool canTopOnFocus);
        bool isCanTopOnFocus() const;
    };
}

#endif //PANEL_H