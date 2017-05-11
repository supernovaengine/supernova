
#include "WAVReader.h"

#include "AudioFile.h"
#include "platform/Log.h"

#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))


AudioFile* WAVReader::getRawAudio(FileData* filedata){

    filedata->read32();
    filedata->read32();
    if (filedata->read32() != MAKEDWORD('W','A','V','E'))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }
    int chunk = filedata->read32();
    if (chunk == MAKEDWORD('J', 'U', 'N', 'K'))
    {
        int size = filedata->read32();
        if (size & 1)
        {
            size += 1;
        }
        int i;
        for (i = 0; i < size; i++)
            filedata->read8();
        chunk = filedata->read32();
    }
    if (chunk != MAKEDWORD('f', 'm', 't', ' '))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }
    unsigned int subchunk1size = filedata->read32();
    unsigned int audioformat = filedata->read16();
    unsigned int channels = filedata->read16();
    unsigned int samplerate = filedata->read32();
    /*int byterate =*/ filedata->read32();
    /*int blockalign =*/ filedata->read16();
    int bitspersample = filedata->read16();

    if (audioformat != 1 ||
        (bitspersample != 8 && bitspersample != 16))
    {
        //return FILE_LOAD_FAILED;
        return NULL;
    }

    if (subchunk1size != 16)
        filedata->seek(filedata->pos() + subchunk1size - 16);

    chunk = filedata->read32();

    if (chunk == MAKEDWORD('L','I','S','T'))
    {
        int size = filedata->read32();
        int i;
        for (i = 0; i < size; i++)
            filedata->read8();
        chunk = filedata->read32();
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

    int subchunk2size = filedata->read32();

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
                    mData[i] = ((signed)filedata->read8() - 128) / (float)0x80;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed)filedata->read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        filedata->read8();
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
                    mData[i] = ((signed short)filedata->read16()) / (float)0x8000;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed short)filedata->read16()) / (float)0x8000;
                    }
                    else
                    {
                        filedata->read16();
                    }
                }
            }
        }
    }

    FileData* data = new FileData((unsigned char*)mData, sizeof(float) * samples * readchannels);

    return new AudioFile(mChannels, bitspersample, samples, samplerate, data);

}