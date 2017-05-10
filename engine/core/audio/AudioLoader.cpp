
#include "AudioLoader.h"

#include <assert.h>
#include "platform/Log.h"

#include "WAVReader.h"
#include "MP3Reader.h"

AudioLoader::AudioLoader() {
}

AudioLoader::AudioLoader(const char* relative_path){
    loadAudio(relative_path);
}

AudioLoader::~AudioLoader() {
}

AudioFile* AudioLoader::loadAudio(const char* relative_path){
    assert(relative_path != NULL);

    Data* datafile = new Data();
    datafile->open(relative_path);
    /*
    if (!fp) {
        Log::Error(LOG_TAG, "Can`t load audio file: %s", relative_path);
    }
    assert(fp);
    */

    AudioReader* audioReader = getAudioFormat(datafile);

    if (audioReader==NULL){
        Log::Error(LOG_TAG, "Not supported audio format from: %s", relative_path);
    }
    assert(audioReader!=NULL);

    datafile->seek(0);

    AudioFile* audioFile;
    audioFile = audioReader->getRawAudio(datafile);

    delete datafile;
    delete audioReader;

    return audioFile;
}

AudioReader* AudioLoader::getAudioFormat(Data* datafile){
    datafile->seek(0);
    int tag = datafile->read32();
    unsigned char bytes[4];

    bytes[0] = tag & 0xFF;
    bytes[1] = (tag >> 8) & 0xFF;
    bytes[2] = (tag >> 16) & 0xFF;
    bytes[3] = (tag >> 24) & 0xFF;

    if (bytes[0]=='R' && bytes[1]=='I' && bytes[2]=='F' && bytes[3]=='F'){
        return new WAVReader();
    }else if (bytes[0]=='O' && bytes[1]=='g' && bytes[2]=='g' && bytes[3]=='S') {
        //Load ogg
    }else if ((bytes[0]=='I' && bytes[1]=='D' && bytes[2]=='3') || (bytes[0]==0xFF && bytes[1]==0xFB)){
        return new MP3Reader();
    }

    return NULL;
}
