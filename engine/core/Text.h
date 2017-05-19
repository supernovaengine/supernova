
#ifndef Text_h
#define Text_h

#include "Mesh2D.h"

class Text: public Mesh2D {

private:
    void createVertices();
    
    std::string font;

public:
    Text();
    Text(std::string font);
    virtual ~Text();
    
    void setFont(std::string font);

    bool load();
};

#endif /* Text_h */
