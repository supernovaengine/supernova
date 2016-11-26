
#include "AudioLoader.h"

#include <assert.h>
#include "platform/Log.h"

#include "WAVReader.h"
#include "MP3Reader.h"

AudioLoader::AudioLoader() {
    audioFile = NULL;
}

AudioLoader::AudioLoader(const char* relative_path){
    loadRawAudio(relative_path);
}

AudioLoader::~AudioLoader() {
    delete audioFile;
}

void AudioLoader::loadRawAudio(const char* relative_path) {
    assert(relative_path != NULL);
    
    FILE* fp = fopen(relative_path,"rb");
    if (!fp) {
        Log::Error(LOG_TAG, "Can`t load audio file: %s", relative_path);
    }
    assert(fp);
    
    AudioReader* audioReader = getAudioFormat(relative_path, fp);
    
    if (audioReader==NULL){
        Log::Error(LOG_TAG, "Not supported audio format from: %s", relative_path);
    }
    assert(audioReader!=NULL);
    
    fseek(fp, 0, SEEK_SET);
    
    audioFile = audioReader->getRawAudio(relative_path, fp);
    
    fclose(fp);
    delete audioReader;
    
}

AudioReader* AudioLoader::getAudioFormat(const char* relative_path, FILE* file){
    
    
    unsigned int magic2, magic3, magic4;
    unsigned char buf[4];
    
    if ((fread(&buf[0], 4, 1, file) != 1) ||
        (fseek(file, 0, SEEK_SET) != 0)) {
        return NULL;
    }
    
    magic4 = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    magic3 = (buf[0] << 16) | (buf[1] << 8) | buf[2];
    
    if (magic3 == 0x494433 || magic2 == 0xFFFB) {
        return new MP3Reader();
    }
    
    if (magic4 == 0x52494646 || magic4 == 0x57415645) {
        return new WAVReader();
    }
    
    return NULL;
}


AudioFile* AudioLoader::getRawAudio(){
    return audioFile;
}
