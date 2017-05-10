#ifndef AudioPlayer_h
#define AudioPlayer_h

#include <string>

class AudioPlayer{
    
protected:
    std::string filename;
    bool isLoaded;
    
public:
    AudioPlayer();
    virtual ~AudioPlayer();
    
    void setFile(std::string filename);
    
    virtual int load() = 0;
    virtual void destroy() = 0;

    virtual int play() = 0;
    virtual int pause() = 0;
    virtual int resume() = 0;
    virtual int stop() = 0;
    
};

#endif
