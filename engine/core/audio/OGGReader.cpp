#include "OGGReader.h"

#include "stb_vorbis.h"

AudioFile* OGGReader::getRawAudio(FileData* filedata){
    int e = 0;
    stb_vorbis *aVorbis = stb_vorbis_open_memory(filedata->getMemPtr(), filedata->length(), &e, 0);
    stb_vorbis_info info = stb_vorbis_get_info(aVorbis);
    unsigned int mBaseSamplerate = (float)info.sample_rate;
    unsigned int samples = stb_vorbis_stream_length_in_samples(aVorbis);

    int readchannels = 1;
    int mChannels = info.channels;
    if (info.channels > 1)
    {
        readchannels = 2;
        mChannels = 2;
    }
    float* mData = new float[samples * readchannels];
    unsigned int mSampleCount = samples;
    unsigned int size = sizeof(float) * mSampleCount * readchannels;
    unsigned int bitspersample = size / mSampleCount * mChannels;
    samples = 0;
    while(1)
    {
        float **outputs;
        int n = stb_vorbis_get_frame_float(aVorbis, NULL, &outputs);
        if (n == 0)
        {
            break;
        }
        if (readchannels == 1)
        {
            memcpy(mData + samples, outputs[0],sizeof(float) * n);
        }
        else
        {
            memcpy(mData + samples, outputs[0],sizeof(float) * n);
            memcpy(mData + samples + mSampleCount, outputs[1],sizeof(float) * n);
        }
        samples += n;
    }

    FileData* data = new FileData((unsigned char*)mData, size);

    stb_vorbis_close(aVorbis);

    return new AudioFile(mChannels, bitspersample, mSampleCount, mBaseSamplerate, data);
}