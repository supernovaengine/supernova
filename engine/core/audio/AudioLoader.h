

#ifndef AudioLoader_h
#define AudioLoader_h

#include "AudioReader.h"
#include "AudioFile.h"
#include <stdio.h>

class AudioLoader{
    
private:
    AudioFile* audioFile;
    
    AudioReader* getAudioFormat(const char* relative_path, FILE* file);
public:
    AudioLoader();
    AudioLoader(const char* relative_path);
    virtual ~AudioLoader();
    
    void loadRawAudio(const char* relative_path);
    
    AudioFile* getRawAudio();
    
};

#endif /* AudioLoader_h */
