

#ifndef WAVReader_h
#define WAVReader_h

#include "AudioReader.h"

class WAVReader: public AudioReader{
    
public:
    AudioFile* getRawAudio(Data* datafile);
};

#endif /* WAVReader_h */
