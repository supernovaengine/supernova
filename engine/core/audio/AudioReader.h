
#ifndef AudioReader_h
#define AudioReader_h

#include "AudioFile.h"
#include <stdio.h>

class AudioReader{
    
    
public:
    
    virtual AudioFile* getRawAudio(const char* relative_path, FILE* file) = 0;
    
    virtual ~AudioReader();
    
};
#endif /* AudioReader_h */
