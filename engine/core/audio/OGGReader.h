
#ifndef OGGReader_H
#define OGGReader_H

#include "AudioReader.h"

namespace Supernova {
    
    class OGGReader: public AudioReader {
    public:
        AudioFile* getRawAudio(FileData* filedata);
    };
    
}


#endif
