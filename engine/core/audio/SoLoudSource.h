
#ifndef SoLoudSource_H
#define SoLoudSource_H

#include "soloud.h"

#include "soloud_file.h"
#include "AudioFile.h"

struct stb_vorbis;

class SoLoudSource;

class SoLoudSourceInstance : public SoLoud::AudioSourceInstance
{
    SoLoudSource *mParent;
	unsigned int mOffset;
public:
	SoLoudSourceInstance(SoLoudSource *aParent);
	virtual void getAudio(float *aBuffer, unsigned int aSamples);
	virtual SoLoud::result rewind();
	virtual bool hasEnded();
};

class SoLoudSource : public SoLoud::AudioSource
{
	SoLoud::result loadmp3(SoLoud::File *aReader);
	SoLoud::result loadwav(SoLoud::File *aReader);
	SoLoud::result loadogg(stb_vorbis *aVorbis);
	SoLoud::result testAndLoadFile(SoLoud::File *aReader);
public:
	float *mData;
	unsigned int mSampleCount;

	SoLoudSource();
	virtual ~SoLoudSource();
	SoLoud::result load(const char *aFilename);
	SoLoud::result load(AudioFile* audioFile);
	SoLoud::result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
	SoLoud::result loadFile(SoLoud::File *aFile);

	virtual SoLoud::AudioSourceInstance *createInstance();
	SoLoud::time getLength();
};


#endif
