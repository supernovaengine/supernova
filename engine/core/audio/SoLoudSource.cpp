

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "soloud.h"
#include "SoLoudSource.h"
#include "stb_vorbis.h"
#include "mpg123.h"


ssize_t s_read(void * handle, void *buffer, size_t size) {
    return ((SoLoud::File*)handle)->read((unsigned char*)buffer, size);
}

off_t s_lseek(void *handle, off_t offset, int whence) {
    switch (whence)
    {
        case SEEK_CUR:
            ((SoLoud::File*)handle)->seek(((SoLoud::File*)handle)->pos() + offset);
            break;
        case SEEK_END:
            ((SoLoud::File*)handle)->seek(((SoLoud::File*)handle)->length() + offset);
            break;
        default:
            ((SoLoud::File*)handle)->seek(offset);
    }
    return ((SoLoud::File*)handle)->pos();
}


void s_cleanup(void *handle) {
    //Not necessary to close handle here
    //fclose((FILE*)handle);
}


SoLoudSourceInstance::SoLoudSourceInstance(SoLoudSource *aParent)
{
    mParent = aParent;
    mOffset = 0;
}

void SoLoudSourceInstance::getAudio(float *aBuffer, unsigned int aSamples)
{
    if (mParent->mData == NULL)
        return;

    // Buffer size may be bigger than samples, and samples may loop..

    unsigned int written = 0;
    unsigned int maxwrite = (aSamples > mParent->mSampleCount) ?  mParent->mSampleCount : aSamples;
    unsigned int channels = mChannels;

    while (written < aSamples)
    {
        unsigned int copysize = maxwrite;
        if (copysize + mOffset > mParent->mSampleCount)
        {
            copysize = mParent->mSampleCount - mOffset;
        }

        if (copysize + written > aSamples)
        {
            copysize = aSamples - written;
        }

        unsigned int i;
        for (i = 0; i < channels; i++)
        {
            memcpy(aBuffer + i * aSamples + written, mParent->mData + mOffset + i * mParent->mSampleCount, sizeof(float) * copysize);
        }

        written += copysize;
        mOffset += copysize;

        if (copysize != maxwrite)
        {
            if (mFlags & SoLoud::AudioSourceInstance::LOOPING)
            {
                if (mOffset == mParent->mSampleCount)
                {
                    mOffset = 0;
                    mLoopCount++;
                }
            }
            else
            {
                for (i = 0; i < channels; i++)
                {
                    memset(aBuffer + copysize + i * aSamples, 0, sizeof(float) * (aSamples - written));
                }
                mOffset += aSamples - written;
                written = aSamples;
            }
        }
    }
}

SoLoud::result SoLoudSourceInstance::rewind()
{
    mOffset = 0;
    mStreamTime = 0;
    return 0;
}

bool SoLoudSourceInstance::hasEnded()
{
    if (!(mFlags & SoLoud::AudioSourceInstance::LOOPING) && mOffset >= mParent->mSampleCount)
    {
        return 1;
    }
    return 0;
}

SoLoudSource::SoLoudSource()
{
    mData = NULL;
    mSampleCount = 0;
}

SoLoudSource::~SoLoudSource()
{
    stop();
    delete[] mData;
}

#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

SoLoud::result SoLoudSource::loadmp3(SoLoud::File *aReader) {

    mpg123_handle *mh;

    int channels, encoding, bitsPerSample;
    long rate;
    size_t size, done;
    off_t numSamples;

    mpg123_init();

    int err = MPG123_OK;

    mh = mpg123_new(NULL, &err);
    if (mh == NULL || err != MPG123_OK)
    {
        return NULL;
    }

    mpg123_replace_reader_handle(mh, s_read, s_lseek, s_cleanup);

    if (mpg123_open_handle(mh, aReader) != MPG123_OK) {
        mpg123_delete(mh);
        return NULL;
    }

    if (mpg123_getformat(mh, &rate, &channels, &encoding) !=  MPG123_OK) {
        mpg123_delete(mh);
        return NULL;
    }

    mpg123_scan(mh);

    numSamples = mpg123_length(mh);
    bitsPerSample = mpg123_encsize(encoding) * 8;

    size = numSamples * channels * mpg123_encsize(encoding);

    int readchannels = 1;
    if (channels > 1)
    {
        readchannels = 2;
        mChannels = 2;
    }
    mData = new float[numSamples * readchannels];
    mBaseSamplerate = (float)rate;
    mSampleCount = numSamples;

    void *data = malloc(size);

    err = mpg123_read(mh, (unsigned char*)data, size, &done );

    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    SoLoud::MemoryFile tempData;
    tempData.openMem((unsigned char*)data, size, false, true);

    int i, j;
    if (bitsPerSample == 8)
    {
        for (i = 0; i < numSamples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    mData[i] = ((signed)tempData.read8() - 128) / (float)0x80;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + numSamples] = ((signed)tempData.read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        tempData.read8();
                    }
                }
            }
        }
    }
    else
    if (bitsPerSample == 16)
    {
        for (i = 0; i < numSamples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    mData[i] = ((signed short)tempData.read16()) / (float)0x8000;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + numSamples] = ((signed short)tempData.read16()) / (float)0x8000;
                    }
                    else
                    {
                        tempData.read16();
                    }
                }
            }
        }
    }

    return 0;

}


SoLoud::result SoLoudSource::loadwav(SoLoud::File *aReader)
{
    /*int wavsize =*/ aReader->read32();
    if (aReader->read32() != MAKEDWORD('W','A','V','E'))
    {
        return SoLoud::FILE_LOAD_FAILED;
    }
    int chunk = aReader->read32();
    if (chunk == MAKEDWORD('J', 'U', 'N', 'K'))
    {
        int size = aReader->read32();
        if (size & 1)
        {
            size += 1;
        }
        int i;
        for (i = 0; i < size; i++)
            aReader->read8();
        chunk = aReader->read32();
    }
    if (chunk != MAKEDWORD('f', 'm', 't', ' '))
    {
        return SoLoud::FILE_LOAD_FAILED;
    }
    int subchunk1size = aReader->read32();
    int audioformat = aReader->read16();
    int channels = aReader->read16();
    int samplerate = aReader->read32();
    /*int byterate =*/ aReader->read32();
    /*int blockalign =*/ aReader->read16();
    int bitspersample = aReader->read16();

    if (audioformat != 1 ||
        (bitspersample != 8 && bitspersample != 16))
    {
        return SoLoud::FILE_LOAD_FAILED;
    }

    if (subchunk1size != 16)
        aReader->seek(aReader->pos() + subchunk1size - 16);

    chunk = aReader->read32();

    if (chunk == MAKEDWORD('L','I','S','T'))
    {
        int size = aReader->read32();
        int i;
        for (i = 0; i < size; i++)
            aReader->read8();
        chunk = aReader->read32();
    }

    if (chunk != MAKEDWORD('d','a','t','a'))
    {
        return SoLoud::FILE_LOAD_FAILED;
    }

    int readchannels = 1;

    if (channels > 1)
    {
        readchannels = 2;
        mChannels = 2;
    }

    int subchunk2size = aReader->read32();

    int samples = (subchunk2size / (bitspersample / 8)) / channels;

    mData = new float[samples * readchannels];

    int i, j;
    if (bitspersample == 8)
    {
        for (i = 0; i < samples; i++)
        {
            for (j = 0; j < channels; j++)
            {
                if (j == 0)
                {
                    mData[i] = ((signed)aReader->read8() - 128) / (float)0x80;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed)aReader->read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        aReader->read8();
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
                    mData[i] = ((signed short)aReader->read16()) / (float)0x8000;
                }
                else
                {
                    if (readchannels > 1 && j == 1)
                    {
                        mData[i + samples] = ((signed short)aReader->read16()) / (float)0x8000;
                    }
                    else
                    {
                        aReader->read16();
                    }
                }
            }
        }
    }
    mBaseSamplerate = (float)samplerate;
    mSampleCount = samples;

    return 0;
}

SoLoud::result SoLoudSource::loadogg(stb_vorbis *aVorbis)
{
    stb_vorbis_info info = stb_vorbis_get_info(aVorbis);
    mBaseSamplerate = (float)info.sample_rate;
    int samples = stb_vorbis_stream_length_in_samples(aVorbis);

    int readchannels = 1;
    if (info.channels > 1)
    {
        readchannels = 2;
        mChannels = 2;
    }
    mData = new float[samples * readchannels];
    mSampleCount = samples;
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
    stb_vorbis_close(aVorbis);

    return 0;
}

SoLoud::result SoLoudSource::testAndLoadFile(SoLoud::File *aReader)
{
    delete[] mData;
    mData = 0;
    mSampleCount = 0;
    int tag = aReader->read32();
    if (tag == MAKEDWORD('O','g','g','S'))
    {
        aReader->seek(0);
        int e = 0;
        stb_vorbis *v = 0;
        v = stb_vorbis_open_file((Soloud_Filehack*)aReader, 0, &e, 0);

        if (0 != v)
        {
            return loadogg(v);
        }
        return SoLoud::FILE_LOAD_FAILED;
    }
    else if (tag == MAKEDWORD('R','I','F','F'))
    {
        return loadwav(aReader);
    }else{
        return loadmp3(aReader);
    }
    return SoLoud::FILE_LOAD_FAILED;
}

SoLoud::result SoLoudSource::load(const char *aFilename)
{
    SoLoud::DiskFile dr;
    int res = dr.open(aFilename);
    if (res != SoLoud::SO_NO_ERROR)
    {
        return res;
    }
    return testAndLoadFile(&dr);
}

SoLoud::result SoLoudSource::load(AudioFile* audioFile){
    mSampleCount = audioFile->getSamples();
    mBaseSamplerate = audioFile->getSampleRate();
    mData = (float*)audioFile->getData();
    mChannels = audioFile->getChannels();

    return 0;
}

SoLoud::result SoLoudSource::loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy, bool aTakeOwnership)
{
    if (aMem == NULL || aLength == 0)
        return SoLoud::INVALID_PARAMETER;

    SoLoud::MemoryFile dr;
    dr.openMem(aMem, aLength, aCopy, aTakeOwnership);
    return testAndLoadFile(&dr);
}

SoLoud::result SoLoudSource::loadFile(SoLoud::File *aFile)
{
    return testAndLoadFile(aFile);
}

SoLoud::AudioSourceInstance *SoLoudSource::createInstance()
{
    return new SoLoudSourceInstance(this);
}

double SoLoudSource::getLength()
{
    if (mBaseSamplerate == 0)
        return 0;
    return mSampleCount / mBaseSamplerate;
}
