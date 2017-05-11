
#ifndef OGGReader_H
#define OGGReader_H

#include "AudioReader.h"

class OGGReader: public AudioReader {
public:
    AudioFile* getRawAudio(FileData* filedata);
};


#endif
