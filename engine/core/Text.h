
#ifndef Text_h
#define Text_h

#include "Mesh2D.h"

class Text: public Mesh2D {

private:
    void createVertices();

public:
    Text();
    Text(std::string text);
    virtual ~Text();

    bool load();
};

#endif /* Text_h */
