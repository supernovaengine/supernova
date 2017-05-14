

#ifndef AudioLoader_h
#define AudioLoader_h

#include "AudioReader.h"
#include "AudioFile.h"
#include <stdio.h>

class AudioLoader{
    
private:
    AudioReader* getAudioFormat(FileData* filedata);
public:
    AudioLoader();
    AudioLoader(const char* relative_path);
    virtual ~AudioLoader();

    AudioFile* loadAudio(const char* relative_path);
    
};

#endif /* AudioLoader_h */
