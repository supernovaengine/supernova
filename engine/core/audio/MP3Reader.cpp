
#include "MP3Reader.h"

#include "mpg123.h"


ssize_t MP3Reader::s_read(void * handle, void *buffer, size_t size) {
    return fread((char*)buffer, 1, size, (FILE*)handle);
}

off_t MP3Reader::s_lseek(void *handle, off_t offset, int whence) {
    if (fseek((FILE*)handle, offset, whence) == 0)
        return ftell((FILE*)handle);

    return (off_t)-1;
}


void MP3Reader::s_cleanup(void *handle) {
    //Not necessary to close handle here
    //fclose((FILE*)handle);
}


AudioFile* MP3Reader::getRawAudio(const char* relative_path, FILE* file){

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

    mpg123_replace_reader_handle(mh, MP3Reader::s_read, MP3Reader::s_lseek, MP3Reader::s_cleanup);

    if (mpg123_open_handle(mh, file) != MPG123_OK) {
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

    void *data = malloc(size);

    err = mpg123_read(mh, (unsigned char*)data, size, &done );

    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    if (err == MPG123_OK || err == MPG123_DONE){

        return new AudioFile(channels, bitsPerSample, size, rate, (void*) data);
    }

    return NULL;

}
