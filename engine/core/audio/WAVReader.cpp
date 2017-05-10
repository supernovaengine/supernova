
#include "WAVReader.h"

#include "AudioFile.h"
#include "platform/Log.h"

#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))


AudioFile* WAVReader::getRawAudio(Data* datafile){

    datafile->read32();
    datafile->read32();
    if (datafile->read32() != MAKEDWORD('W','A','V','E'))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }
    int chunk = datafile->read32();
    if (chunk == MAKEDWORD('J', 'U', 'N', 'K'))
    {
        int size = datafile->read32();
        if (size & 1)
        {
            size += 1;
        }
        int i;
        for (i = 0; i < size; i++)
            datafile->read8();
        chunk = datafile->read32();
    }
    if (chunk != MAKEDWORD('f', 'm', 't', ' '))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }
    int subchunk1size = datafile->read32();
    int audioformat = datafile->read16();
    int channels = datafile->read16();
    int samplerate = datafile->read32();
    /*int byterate =*/ datafile->read32();
    /*int blockalign =*/ datafile->read16();
    int bitspersample = datafile->read16();

    if (audioformat != 1 ||
        (bitspersample != 8 && bitspersample != 16))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }

    if (subchunk1size != 16)
        datafile->seek(datafile->pos() + subchunk1size - 16);

    chunk = datafile->read32();

    if (chunk == MAKEDWORD('L','I','S','T'))
    {
        int size = datafile->read32();
        int i;
        for (i = 0; i < size; i++)
            datafile->read8();
        chunk = datafile->read32();
    }

    if (chunk != MAKEDWORD('d','a','t','a'))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }

    int readchannels = 1;
    int mChannels = channels;
    if (channels > 1)
    {
        readchannels = 2;
        mChannels = 2;
    }

    int subchunk2size = datafile->read32();

    int samples = (subchunk2size / (bitspersample / 8)) / channels;

    float* mData = new float[samples * readchannels];

    int i, j;
    if (bitspersample == 8)
    {
        for (i = 0; i < samples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    mData[i] = ((signed)datafile->read8() - 128) / (float)0x80;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed)datafile->read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        datafile->read8();
                    }
                }
            }
        }
    }
    else
    if (bitspersample == 16)
    {
        for (i = 0; i < samples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    mData[i] = ((signed short)datafile->read16()) / (float)0x8000;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed short)datafile->read16()) / (float)0x8000;
                    }
                    else
                    {
                        datafile->read16();
                    }
                }
            }
        }
    }

    return new AudioFile(mChannels, bitspersample, samples, subchunk2size, samplerate, (void*) mData);

}