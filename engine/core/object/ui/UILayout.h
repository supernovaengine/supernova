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

        void setAnchorPoints(float left, float top, float right, float bottom);
        void setAnchorPointLeft(float left);
        void setAnchorPointTop(float top);
        void setAnchorPointRight(float right);
        void setAnchorPointBottom(float bottom);

        float getAnchorPointLeft() const;
        float getAnchorPointTop() const;
        float getAnchorPointRight() const;
        float getAnchorPointBottom() const;

        void setAnchorOffsets(int left, int top, int right, int bottom);
        void setAnchorOffsetLeft(int left);
        void setAnchorOffsetTop(int top);
        void setAnchorOffsetRight(int right);
        void setAnchorOffsetBottom(int bottom);

        int getAnchorOffsetLeft() const;
        int getAnchorOffsetTop() const;
        int getAnchorOffsetRight() const;
        int getAnchorOffsetBottom() const;

        void setAnchorPreset(AnchorPreset anchorPreset);
        AnchorPreset getAnchorPreset() const;

        void setUsingAnchors(bool usingAnchors);
        bool isUsingAnchors() const;

        void setIgnoreScissor(bool ignoreScissor);
        bool isIgnoreScissor() const;
    };
}

#endif //UILAYOUT_H