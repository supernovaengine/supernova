#ifndef SupernovaIOS_h
#define SupernovaIOS_h

#include "system/System.h"

class SupernovaIOS: public Supernova::System{
public:
    static int screenWidth;
    static int screenHeight;
    
    virtual int getScreenWidth();
    virtual int getScreenHeight();
    
    virtual void showVirtualKeyboard();
    virtual void hideVirtualKeyboard();
    
    virtual std::string getAssetPath();
    virtual std::string getUserDataPath();
};


#endif /* SupernovaIOS_h */
