
#include "WAVReader.h"

#include "AudioFile.h"
#include <string>
#include <stdlib.h>
#include "platform/Log.h"

typedef struct header_file
{
    char chunk_id[4];
    int chunk_size;
    char format[4];
    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;
    int byte_rate;
    short int block_align;
    short int bits_per_sample;
    char subchunk2_id[4];
    int subchunk2_size;
} header;

typedef struct header_file* header_p;


AudioFile* WAVReader::getRawAudio(const char* relative_path, FILE* file){
    //FILE* fp = fopen(fname,"rb");
    if (file) {
        char chunk_id[5], format[5];
        short* bufferData;
        int data_size;

        header_p meta = (header_p)malloc(sizeof(header));
        fread(meta, 1, sizeof(header), file);

        for (int i=0; i<4; i++) {
            chunk_id[i] = meta->chunk_id[i];
            format[i] = meta->format[i];
        }
        chunk_id[4] = '\0';
        format[4] = '\0';
        data_size = meta->subchunk2_size;

        if (!strcmp(chunk_id, "RIFF") && !strcmp(format,"WAVE")){
            bufferData = (short*) malloc(data_size);
            fread(bufferData, sizeof(short), data_size/sizeof(short), file);

            return new AudioFile(meta->num_channels, meta->bits_per_sample, data_size, meta->sample_rate, (void*) bufferData);
        }else {
            Log::Error(LOG_TAG, "RIFF file but not a wave file!");
        }


    }
    
    return NULL;
    //fclose(fp);
}
