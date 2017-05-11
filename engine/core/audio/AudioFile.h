
#ifndef AudioFile_h
#define AudioFile_h

#include "file/FileData.h"

class AudioFile {
private:

    unsigned int channels;
    unsigned int bitsPerSample;
    unsigned int samples;
    unsigned int sampleRate;
    FileData* filedata;
    bool dataOwned;
    
public:
    
    AudioFile();
    AudioFile(unsigned int channels, unsigned int bitsPerSample, unsigned int samples, unsigned int sampleRate, FileData* filedata);
    AudioFile(const AudioFile& v);
    AudioFile& operator = ( const AudioFile& v );
    virtual ~AudioFile();
    void copy(const AudioFile& v);
    
    void releaseAudioData();

    unsigned int getChannels();
    unsigned int getBitsPerSample();
    unsigned int getSamples();
    unsigned int getSampleRate();
    FileData* getFileData();
    
};

#endif /* AudioFile_h */
