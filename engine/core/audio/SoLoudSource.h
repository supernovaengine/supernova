
#ifndef SoLoudSource_H
#define SoLoudSource_H

#include "soloud.h"

#include "AudioFile.h"

struct stb_vorbis;

class SoLoudSource;

class SoLoudSourceInstance : public SoLoud::AudioSourceInstance {
    SoLoudSource *mParent;
	unsigned int mOffset;
public:
	SoLoudSourceInstance(SoLoudSource *aParent);
	virtual void getAudio(float *aBuffer, unsigned int aSamples);
	virtual SoLoud::result rewind();
	virtual bool hasEnded();
};

class SoLoudSource : public SoLoud::AudioSource {
public:
	float *mData;
	unsigned int mSampleCount;

	SoLoudSource();
	virtual ~SoLoudSource();
	SoLoud::result load(AudioFile* audioFile);

	virtual SoLoud::AudioSourceInstance *createInstance();
	SoLoud::time getLength();
};


#endif
