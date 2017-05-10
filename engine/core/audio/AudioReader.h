
#ifndef AudioReader_h
#define AudioReader_h

#include "AudioFile.h"
#include "SoLoudSource.h"
#include "Data.h"
#include <stdio.h>

class AudioReader{
    
    
public:
    
    virtual AudioFile* getRawAudio(Data* datafile) = 0;
    
    virtual ~AudioReader();
    
};
#endif /* AudioReader_h */
