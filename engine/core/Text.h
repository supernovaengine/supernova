
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
        
    protected:
        void createText();

    public:
        Text();
        Text(const char* font);
        virtual ~Text();
        
        Text& operator = ( const char* v );
        bool operator == ( const char* v ) const;
        bool operator != ( const char* v ) const;
        
        virtual void setSize(int width, int height);
        virtual void setInvert(bool invert);

        void setMinBufferSize(unsigned int characters);
        
        void setFont(const char* font);
        void setText(const char* text);
        void setFontSize(unsigned int fontSize);

        virtual bool load();
    };
    
}

#endif /* Text_h */
