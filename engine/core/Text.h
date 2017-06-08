
#ifndef Text_h
#define Text_h

#include "Mesh2D.h"

class STBText;

class Text: public Mesh2D {

private:
    
    STBText* stbtext;

    std::string font;
    std::string text;

public:
    Text();
    Text(std::string font);
    virtual ~Text();
    
    Text& operator = ( const char* v );
    bool operator == ( const char* v ) const;
    bool operator != ( const char* v ) const;
    
    void setSize(int width, int height);
    
    void setFont(std::string font);
    void setText(std::string text);
    
    void drawText();

    virtual bool load();
};

#endif /* Text_h */
