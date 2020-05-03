#ifndef SupernovaWeb_h
#define SupernovaWeb_h

#include "system/System.h"

class SupernovaWeb: public Supernova::System{
public:

    static int screenWidth;
    static int screenHeight;

    SupernovaWeb();

    virtual int getScreenWidth();
    virtual int getScreenHeight();
    
};


#endif /* SupernovaWeb_h */