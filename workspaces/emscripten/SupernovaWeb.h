#ifndef SupernovaWeb_h
#define SupernovaWeb_h

#include "platform/SystemPlatform.h"

class SupernovaWeb: public Supernova::SystemPlatform{
public:
    virtual void showVirtualKeyboard() {};
    virtual void hideVirtualKeyboard() {};
    virtual FILE* platformFopen(const char* fname, const char* mode);
};


#endif /* SupernovaWeb_h */