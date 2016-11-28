
#include "OpenalPlayer.h"


#include "AudioLoader.h"

OpenalPlayer::OpenalPlayer(): AudioPlayer(){
}

OpenalPlayer::~OpenalPlayer(){
    destroy();
}

ALenum OpenalPlayer::toAlFormat(short channels, short samples)
{
    bool stereo = (channels > 1);
    
    switch (samples) {
        case 16:
            if (stereo)
                return AL_FORMAT_STEREO16;
            else
                return AL_FORMAT_MONO16;
        case 8:
            if (stereo)
                return AL_FORMAT_STEREO8;
            else
                return AL_FORMAT_MONO8;
        default:
            return -1;
    }
}

int OpenalPlayer::test_error(const char* _msg){
    int error = alGetError();
    if (error != AL_NO_ERROR) {
        fprintf(stderr, "%s\n", _msg);
        return -1;
    }
    return 0;
}

int OpenalPlayer::load(){
    if (!isLoaded) {

        ALboolean enumeration;
        const ALCchar *defaultDeviceName = "";

        enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
        if (enumeration == AL_FALSE)
            fprintf(stderr, "enumeration extension not available\n");


        if (!defaultDeviceName)
            defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

        device = alcOpenDevice(defaultDeviceName);
        if (!device) {
            fprintf(stderr, "unable to open default device\n");
            return -1;
        }

        //fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

        alGetError();

        context = alcCreateContext(device, NULL);
        if (!alcMakeContextCurrent(context)) {
            fprintf(stderr, "failed to make default context\n");
            return -1;
        }
        test_error("make default context");


        alGenBuffers(1, &buffer);
        test_error("buffer generation");

        //Loading audio data
        AudioLoader audioLoader(filename.c_str());

        alBufferData(buffer, toAlFormat(audioLoader.getRawAudio()->getChannels(),
                                        audioLoader.getRawAudio()->getBitsPerSample()),
                     audioLoader.getRawAudio()->getData(), audioLoader.getRawAudio()->getSize(),
                     audioLoader.getRawAudio()->getSampleRate());
        test_error("failed to load buffer data");

        audioLoader.getRawAudio()->releaseAudioData();

        isLoaded = true;
    }
    return 0;
}


int OpenalPlayer::play(){
    
    if (!isLoaded)
        load();

    ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    test_error("listener position");
    alListener3f(AL_VELOCITY, 0, 0, 0);
    test_error("listener velocity");
    alListenerfv(AL_ORIENTATION, listenerOri);
    test_error("listener orientation");
    
    alGenSources((ALuint)1, &source);
    test_error("source generation");
    
    alSourcef(source, AL_PITCH, 1);
    test_error("source pitch");
    alSourcef(source, AL_GAIN, 1);
    test_error("source gain");
    alSource3f(source, AL_POSITION, 0, 0, 0);
    test_error("source position");
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    test_error("source velocity");
    alSourcei(source, AL_LOOPING, AL_FALSE);
    test_error("source looping");

    
    alSourcei(source, AL_BUFFER, buffer);
    test_error("buffer binding");
    
    alSourcePlay(source);
    test_error("source playing");

    /*
     ALint source_state;
     alGetSourcei(source, AL_SOURCE_STATE, &source_state);
     test_error("source state get");
     while (source_state == AL_PLAYING) {
     alGetSourcei(source, AL_SOURCE_STATE, &source_state);
     test_error("source state get");
     }
     */

    return 0;
    
}

int OpenalPlayer::stop(){
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if(source_state==AL_PLAYING)
        alSourceStop(source);
    
    return 0;
}

void OpenalPlayer::destroy(){
    isLoaded = false;
    stop();
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
