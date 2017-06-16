

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "soloud.h"
#include "SoLoudSource.h"

using namespace Supernova;

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

SoLoudSource::SoLoudSource() {
    mData = NULL;
    mSampleCount = 0;
}

SoLoudSource::~SoLoudSource() {
    stop();
    delete[] mData;
}

SoLoud::result SoLoudSource::load(AudioFile* audioFile){
    mSampleCount = audioFile->getSamples();
    mBaseSamplerate = audioFile->getSampleRate();
    mData = (float*)audioFile->getFileData()->getMemPtr();
    mChannels = audioFile->getChannels();

    return 0;
}

SoLoud::AudioSourceInstance *SoLoudSource::createInstance() {
    return new SoLoudSourceInstance(this);
}

double SoLoudSource::getLength() {
    if (mBaseSamplerate == 0)
        return 0;
    return mSampleCount / mBaseSamplerate;
}
