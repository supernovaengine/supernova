
#ifndef AudioFile_h
#define AudioFile_h

class AudioFile {
private:
    
    int channels;
    int bitsPerSample;
    unsigned long samples;
    unsigned long size;
    int sampleRate;
    void *data;
    bool dataOwned;
    
public:
    
    AudioFile();
    AudioFile(int channels, int bitsPerSample, unsigned long samples, unsigned long size, int sampleRate, void* data);
    AudioFile(const AudioFile& v);
    AudioFile& operator = ( const AudioFile& v );
    virtual ~AudioFile();
    void copy(const AudioFile& v);
    
    void releaseAudioData();
    
    int getChannels();
    int getBitsPerSample();
    unsigned long getSamples();
    unsigned long getSize();
    int getSampleRate();
    void* getData();
    
};

#endif /* AudioFile_h */
