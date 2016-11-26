

#ifndef OpenalPlayer_h
#define OpenalPlayer_h

#include "AudioPlayer.h"

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

class OpenalPlayer: public AudioPlayer{
    
private:
    
    ALCdevice *device;
    ALCcontext *context;
    ALuint buffer, source;
    
    ALenum toAlFormat(short channels, short samples);
    int test_error(const char* _msg);
    
public:
    OpenalPlayer();
    virtual ~OpenalPlayer();
    
    int load();
    int play();
    int stop();
    void destroy();
    
};


#endif /* OpenalPlayer_h */
