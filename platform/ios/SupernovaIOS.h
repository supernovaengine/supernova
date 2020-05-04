#ifndef SupernovaIOS_h
#define SupernovaIOS_h

#include "system/System.h"

class SupernovaIOS: public Supernova::System{
private:
    static const char* getFullPath(const char* filename);
    
public:
    static int screenWidth;
    static int screenHeight;
    
    virtual int getScreenWidth();
    virtual int getScreenHeight();
    
    virtual void showVirtualKeyboard();
    virtual void hideVirtualKeyboard();
    
    virtual FILE* platformFopen(const char* fname, const char* mode);
};


#endif /* SupernovaIOS_h */
