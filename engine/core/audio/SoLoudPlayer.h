
#ifndef SOLOUDPLAYER_H
#define SOLOUDPLAYER_H

#include "AudioPlayer.h"
#include "SoLoudSource.h"
#include "soloud.h"


class SoLoudPlayer: public AudioPlayer {
private:
    SoLoud::Soloud soloud;
    SoLoudSource sample;
    SoLoud::result loaded;

    AudioFile* audioFile;
public:
    SoLoudPlayer();
    virtual ~SoLoudPlayer();

    int load();
    int play();
    int pause();
    int resume();
    int stop();
    void destroy();
};


#endif //SOLOUDPLAYER_H
