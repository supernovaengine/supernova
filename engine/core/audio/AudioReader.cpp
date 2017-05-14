
#include "AudioReader.h"


AudioReader::~AudioReader(){
    
}


void AudioReader::splitAudioChannels(File* file, unsigned int bitsPerSample, unsigned int samples, unsigned int channels, unsigned int outputChannels, float** output){
    int i, j;
    if (bitsPerSample == 8)
    {
        for (i = 0; i < samples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    (*output)[i] = ((signed)file->read8() - 128) / (float)0x80;
                }
                else
                {
                    if (outputChannels > 1 && j == 1)
                    {
                        (*output)[i + samples] = ((signed)file->read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        file->read8();
                    }
                }
            }
        }
    }
    else
    if (bitsPerSample == 16)
    {
        for (i = 0; i < samples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    (*output)[i] = ((signed short)file->read16()) / (float)0x8000;
                }
                else
                {
                    if (outputChannels > 1 && j == 1)
                    {
                        (*output)[i + samples] = ((signed short)file->read16()) / (float)0x8000;
                    }
                    else
                    {
                        file->read16();
                    }
                }
            }
        }
    }
}