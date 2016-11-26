

#ifndef WAVReader_h
#define WAVReader_h

#include "AudioReader.h"

class WAVReader: public AudioReader{
    
public:
    AudioFile* getRawAudio(const char* relative_path, FILE* file);
    
};

#endif /* WAVReader_h */
