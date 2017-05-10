
#include "AudioFile.h"

#include <stdlib.h>
#include <string.h>

AudioFile::AudioFile(){
    this->channels = 0;
    this->bitsPerSample = 0;
    this->samples = 0;
    this->size = 0;
    this->sampleRate = 0;
    this->dataOwned = true;
    this->data = NULL;
}

AudioFile::AudioFile(int channels, int bitsPerSample, unsigned long samples, unsigned long size, int sampleRate, void* data){
    this->channels = channels;
    this->bitsPerSample = bitsPerSample;
    this->samples = samples;
    this->size = size;
    this->sampleRate = sampleRate;
    this->dataOwned = true;
    this->data = data;
}

AudioFile::AudioFile(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    this->data = v.data;
}

AudioFile& AudioFile::operator = ( const AudioFile& v ){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    this->data = v.data;

    return *this;
}

void AudioFile::copy(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->size = v.size;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    
    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

AudioFile::~AudioFile(){
    if (dataOwned){
        releaseAudioData();
    }
}

void AudioFile::releaseAudioData(){
    delete[] data;
}

int AudioFile::getChannels(){
    return channels;
}

int AudioFile::getBitsPerSample(){
    return bitsPerSample;
}

unsigned long AudioFile::getSamples(){
    return samples;
}

unsigned long AudioFile::getSize(){
    return size;
}

int AudioFile::getSampleRate(){
    return sampleRate;
}

void* AudioFile::getData(){
    return data;
}
