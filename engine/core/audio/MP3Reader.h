

#ifndef MP3Reader_h
#define MP3Reader_h

#include "AudioReader.h"
#include <string>

class MP3Reader: public AudioReader{
private:
    static ssize_t s_read(void * handle, void *buffer, size_t size);
    static off_t s_lseek(void *handle, off_t offset, int whence);
    static void s_cleanup(void *handle);
public:
    AudioFile* getRawAudio(Data* datafile);
};

#endif /* MP3Reader_h */
