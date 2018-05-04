#ifndef SupernovaIOS_h
#define SupernovaIOS_h

#include "platform/SystemPlatform.h"

class SupernovaIOS: public Supernova::SystemPlatform{
private:
    static const char* getFullPath(const char* filename);
public:
    virtual void showVirtualKeyboard();
    virtual void hideVirtualKeyboard();
    virtual FILE* platformFopen(const char* fname, const char* mode);
};


#endif /* SupernovaIOS_h */
