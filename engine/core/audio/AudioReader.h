
#ifndef AudioReader_h
#define AudioReader_h

#include "AudioFile.h"
#include "SoLoudSource.h"
#include "FileData.h"
#include <stdio.h>

class AudioReader{
    
    
public:
    
    virtual AudioFile* getRawAudio(FileData* filedata) = 0;
    
    virtual ~AudioReader();
    
};
#endif /* AudioReader_h */
