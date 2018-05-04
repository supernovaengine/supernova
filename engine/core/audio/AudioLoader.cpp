
#include "AudioLoader.h"

#include <assert.h>
#include "Log.h"

#include "WAVReader.h"
#include "MP3Reader.h"
#include "OGGReader.h"

using namespace Supernova;

AudioLoader::AudioLoader() {
}

AudioLoader::AudioLoader(const char* relative_path){
    loadAudio(relative_path);
}

AudioLoader::~AudioLoader() {
}

AudioFile* AudioLoader::loadAudio(const char* relative_path){
    assert(relative_path != NULL);

    FileData* filedata = new FileData();
    filedata->open(relative_path);
    /*
    if (!fp) {
        Log::Error("Can`t load audio file: %s", relative_path);
    }
    assert(fp);
    */

    AudioReader* audioReader = getAudioFormat(filedata);

    if (audioReader==NULL){
        Log::Error("Not supported audio format from: %s", relative_path);
    }
    assert(audioReader!=NULL);

    filedata->seek(0);

    AudioFile* audioFile = audioReader->getRawAudio(filedata);

    delete filedata;
    delete audioReader;

    return audioFile;
}

AudioReader* AudioLoader::getAudioFormat(FileData* filedata){
    filedata->seek(0);
    int tag = filedata->read32();
    unsigned char bytes[4];

    bytes[0] = tag & 0xFF;
    bytes[1] = (tag >> 8) & 0xFF;
    bytes[2] = (tag >> 16) & 0xFF;
    bytes[3] = (tag >> 24) & 0xFF;

    if (bytes[0]=='R' && bytes[1]=='I' && bytes[2]=='F' && bytes[3]=='F'){
        return new WAVReader();
    }else if (bytes[0]=='O' && bytes[1]=='g' && bytes[2]=='g' && bytes[3]=='S') {
        return new OGGReader();
    }else if ((bytes[0]=='I' && bytes[1]=='D' && bytes[2]=='3') || (bytes[0]==0xFF && bytes[1]==0xFB)){
        return new MP3Reader();
    }

    return NULL;
}
