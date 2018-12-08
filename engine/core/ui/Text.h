
#ifndef Text_h
#define Text_h

#include "Mesh2D.h"

namespace Supernova {

    class STBText;

    class Text: public Mesh2D {

    private:
        
        STBText* stbtext;

        std::string font;
        std::string text;
        unsigned int fontSize;
        bool multiline;

        std::vector<Vector2> charPositions;
        
        bool userDefinedWidth;
        bool userDefinedHeight;
        
    protected:
        void createText();

    public:
        Text();
        Text(std::string font);
        virtual ~Text();
        
        Text& operator = ( const char* v );
        bool operator == ( const char* v ) const;
        bool operator != ( const char* v ) const;
        
        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);
        
        void setWidth(int width);
        void setHeight(int height);

        void setMinBufferSize(unsigned int characters);

        float getAscent();
        float getDescent();
        float getLineGap();
        int getLineHeight();
        std::string getFont();
        std::string getText();
        unsigned int getNumChars();
        Vector2 getCharPosition(unsigned int index);
        
        void setFont(std::string font);
        void setText(std::string text);
        void setFontSize(unsigned int fontSize);
        void setMultiline(bool multiline);

        virtual bool load();
    };
    
}

#endif /* Text_h */
