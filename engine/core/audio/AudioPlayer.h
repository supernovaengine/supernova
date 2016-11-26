#ifndef AudioPlayer_h
#define AudioPlayer_h

class AudioPlayer{
    
protected:
    
    const char* filename;
    bool isLoaded;
    
public:
    AudioPlayer();
    virtual ~AudioPlayer();
    
    void setFile(const char* filename);
    
    virtual int load() = 0;
    virtual int play() = 0;
    virtual int stop() = 0;
    virtual void destroy() = 0;
    
};

#endif
