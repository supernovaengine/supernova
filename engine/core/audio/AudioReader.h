
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

    void splitAudioChannels(File* file, unsigned int bitsPerSample, unsigned int samples, unsigned int channels, unsigned int outputChannels, float** output);
    
};
#endif /* AudioReader_h */
