
#ifndef SoLoudLoaderWAV_H
#define SoLoudLoaderWAV_H

#include "soloud.h"

struct stb_vorbis;

namespace SoLoud
{
	class Wav;
	class File;

	class WavInstance : public AudioSourceInstance
	{
		Wav *mParent;
		unsigned int mOffset;
	public:
		WavInstance(Wav *aParent);
		virtual void getAudio(float *aBuffer, unsigned int aSamples);
		virtual result rewind();
		virtual bool hasEnded();
	};

	class Wav : public AudioSource
	{
		result loadmp3(File *aReader);
		result loadwav(File *aReader);
		result loadogg(stb_vorbis *aVorbis);
		result testAndLoadFile(File *aReader);
	public:
		float *mData;
		unsigned int mSampleCount;

		Wav();
		virtual ~Wav();
		result load(const char *aFilename);
		result loadMem(unsigned char *aMem, unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
		result loadFile(File *aFile);
		
		virtual AudioSourceInstance *createInstance();
		time getLength();
	};
};

#endif
