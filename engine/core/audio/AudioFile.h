
#ifndef AudioFile_h
#define AudioFile_h

class AudioFile {
private:
    
    int channels;
    int bitsPerSample;
    int size;
    int sampleRate;
    void *data;
    
public:
    
    AudioFile();
    AudioFile(int channels, int bitsPerSample, unsigned long size, unsigned long sampleRate, void* data);
    AudioFile(const AudioFile& v);
    AudioFile& operator = ( const AudioFile& v );
    virtual ~AudioFile();
    void copy(const AudioFile& v);
    
    void releaseAudioData();
    
    int getChannels();
    int getBitsPerSample();
    int getSize();
    int getSampleRate();
    void* getData();
    
};

#endif /* AudioFile_h */
