#include "OGGReader.h"

#include <string.h>
#include "stb_vorbis.h"

using namespace Supernova;

AudioFile* OGGReader::getRawAudio(FileData* filedata){
    int e = 0;
    stb_vorbis *aVorbis = stb_vorbis_open_memory(filedata->getMemPtr(), filedata->length(), &e, 0);
    stb_vorbis_info info = stb_vorbis_get_info(aVorbis);
    unsigned int mBaseSamplerate = (float)info.sample_rate;
    unsigned int samples = stb_vorbis_stream_length_in_samples(aVorbis);

    int mChannels = 1;
    if (info.channels > 1)
    {
        mChannels = 2;
    }
    unsigned int mSampleCount = samples;
    float* mData = new float[mSampleCount * mChannels];
    unsigned int size = sizeof(float) * mSampleCount * mChannels;
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
        if (mChannels == 1)
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

    stb_vorbis_close(aVorbis);

    FileData* data = new FileData((unsigned char*)mData, size);

    return new AudioFile(mChannels, bitspersample, mSampleCount, mBaseSamplerate, data);
}
