
#include "AudioFile.h"

#include <stdlib.h>
#include <string.h>

using namespace Supernova;

AudioFile::AudioFile(){
    this->channels = 0;
    this->bitsPerSample = 0;
    this->samples = 0;
    this->sampleRate = 0;
    this->dataOwned = true;
    this->filedata = NULL;
}

AudioFile::AudioFile(unsigned int channels, unsigned int bitsPerSample, unsigned int samples, unsigned int sampleRate, FileData* filedata){
    this->channels = channels;
    this->bitsPerSample = bitsPerSample;
    this->samples = samples;
    this->sampleRate = sampleRate;
    this->dataOwned = true;
    this->filedata = filedata;
}

AudioFile::AudioFile(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    this->filedata = v.filedata;
}

AudioFile& AudioFile::operator = ( const AudioFile& v ){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    this->filedata = v.filedata;

    return *this;
}

void AudioFile::copy(const AudioFile& v){
    this->channels = v.channels;
    this->bitsPerSample = v.bitsPerSample;
    this->samples = v.samples;
    this->sampleRate = v.sampleRate;
    this->dataOwned = v.dataOwned;
    this->filedata = new FileData(v.filedata->getMemPtr(), v.filedata->length(), true);
}

AudioFile::~AudioFile(){
    if (dataOwned){
        releaseAudioData();
    }
}

void AudioFile::releaseAudioData(){
    delete filedata;
}

unsigned int AudioFile::getChannels(){
    return channels;
}

unsigned int AudioFile::getBitsPerSample(){
    return bitsPerSample;
}

unsigned int AudioFile::getSamples(){
    return samples;
}

unsigned int AudioFile::getSampleRate(){
    return sampleRate;
}

FileData* AudioFile::getFileData(){
    return filedata;
}
