#ifndef SupernovaSokol_h
#define SupernovaSokol_h

#include "System.h"

class SupernovaSokol: public Supernova::System{

public:

    SupernovaSokol();

    virtual int getScreenWidth();
    virtual int getScreenHeight();

    virtual sg_context_desc getSokolContext();

    virtual bool isFullscreen();
    virtual void requestFullscreen();
    virtual void exitFullscreen();

    virtual std::string getAssetPath();
    virtual std::string getUserDataPath();
    virtual std::string getLuaPath();
    
};


#endif /* SupernovaSokol_h */