
#include "AudioFile.h"

#include <stdlib.h>
#include <string.h>

AudioFile::AudioFile(){
    this->channels = 0;
    this->bitsPerSample = 0;
    this->size = 0;
    this->sampleRate = 0;
    this->data = NULL;
    
}

AudioFile::AudioFile(int channels, int bitsPerSample, unsigned long size, unsigned long sampleRate, void* data){
    this->channels = channels;
    this->bitsPerSample = bitsPerSample;
    this->size = size;
    this->sampleRate = sampleRate;
    this->data = data;
}

AudioFile::AudioFile(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    this->data = v.data;
}

AudioFile& AudioFile::operator = ( const AudioFile& v ){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    this->data = v.data;
    
    return *this;
}

void AudioFile::copy(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    
    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

AudioFile::~AudioFile(){
    
}

void AudioFile::releaseAudioData(){
    free((void*)data);
}

int AudioFile::getChannels(){
    return channels;
}

int AudioFile::getBitsPerSample(){
    return bitsPerSample;
}

int AudioFile::getSize(){
    return size;
}

int AudioFile::getSampleRate(){
    return sampleRate;
}

void* AudioFile::getData(){
    return data;
}
