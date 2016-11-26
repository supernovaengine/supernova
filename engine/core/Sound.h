

#ifndef Sound_h
#define Sound_h

#include <stdio.h>
#include "audio/AudioLoader.h"

#ifdef SUPERNOVA_ANDROID
#include <AL/al.h>
#include <AL/alc.h>
#endif
#ifdef SUPERNOVA_WEB
#include <AL/al.h>
#include <AL/alc.h>
#endif
#ifdef SUPERNOVA_IOS
#import <OpenAl/al.h>
#import <OpenAl/alc.h>
#endif

class Sound{
    
private:
    
    const char* filename;
    bool isLoaded;
    
    ALCdevice *device;
    ALCcontext *context;
    ALuint buffer, source;
    
    ALenum toAlFormat(short channels, short samples);
    int test_error(const char* _msg);
    
public:
    Sound(const char* filename);
    virtual ~Sound();

    int load();
    int play();
    int stop();
    void destroy();

};

#endif /* Sound_h */
