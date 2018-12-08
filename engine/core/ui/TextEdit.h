#ifndef TEXTEDIT_H
#define TEXTEDIT_H

//
// (c) 2018 Eduardo Doria.
//

#include "UIImage.h"
#include "Text.h"
#include "Polygon.h"

namespace Supernova {

    class TextEdit: public UIImage {
    private:
        Text text;
        Polygon cursor;

        float cursorBlinkTimer;
        Vector2 cursorSize;

    protected:
        void adjustText();

    public:
        TextEdit();
        virtual ~TextEdit();

        void setText(std::string text);
        void setTextFont(std::string font);
        void setTextSize(unsigned int size);
        void setTextColor(Vector4 color);
        std::string getText();
        Text* getTextObject();

        virtual void engineOnDown(int pointer, float x, float y);
        virtual void engineOnUp(int pointer, float x, float y);
        virtual void engineOnTextInput(std::string text);

        virtual bool load();
        virtual bool draw();
    };

}


#endif //TEXTEDIT_H
