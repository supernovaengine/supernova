//
// (c) 2022 Eduardo Doria.
//

#ifndef UILAYOUT_H
#define UILAYOUT_H

#include "Object.h"

namespace Supernova{
    class UILayout: public Object{

    public:
        UILayout(Scene* scene);
        UILayout(Scene* scene, Entity entity);

        void setSize(int width, int height);
        void setWidth(int width);
        void setHeight(int height);

        int getWidth() const;
        int getHeight() const;

        void setAnchors(float anchorLeft, float anchorTop, float anchorRight, float anchorBottom);
        void setAnchorLeft(float anchorLeft);
        void setAnchorTop(float anchorTop);
        void setAnchorRight(float anchorRight);
        void setAnchorBottom(float anchorBottom);

        float getAnchorLeft() const;
        float getAnchorTop() const;
        float getAnchorRight() const;
        float getAnchorBottom() const;

        void setMargins(int marginLeft, int marginTop, int marginRight, int marginBottom);
        void setMarginLeft(int marginLeft);
        void setMarginTop(int marginTop);
        void setMarginRight(int marginRight);
        void setMarginBottom(int marginBottom);

        int getMarginLeft() const;
        int getMarginTop() const;
        int getMarginRight() const;
        int getMarginBottom() const;

        void setAnchorPreset(AnchorPreset anchorPreset);
        AnchorPreset getAnchorPreset() const;
    };
}

#endif //UILAYOUT_H