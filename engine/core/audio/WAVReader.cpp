
#include "WAVReader.h"

#include "AudioFile.h"
#include <stdint.h>
#include <string>
#include <stdlib.h>


AudioFile* WAVReader::getRawAudio(const char* relative_path, FILE* file){
    //FILE* fp = fopen(fname,"rb");
    if (file) {
        char id[5];
        short* bufferData;
        unsigned long size;
        short format_tag, channels, block_align, bits_per_sample;
        unsigned long format_length, sample_rate, avg_bytes_sec, data_size;
        
        fread(id, sizeof(uint8_t), 4, file);
        id[4] = '\0';
        
        if (!strcmp(id, "RIFF")) {
            fread(&size, sizeof(uint32_t), 1, file);
            fread(id, sizeof(uint8_t), 4, file);
            id[4] = '\0';
            
            if (!strcmp(id,"WAVE")) {
                fread(id, sizeof(uint8_t), 4, file);
                fread(&format_length, sizeof(uint32_t),1,file);
                fread(&format_tag, sizeof(uint16_t), 1, file);
                fread(&channels, sizeof(uint16_t),1,file);
                fread(&sample_rate, sizeof(uint32_t), 1, file);
                fread(&avg_bytes_sec, sizeof(uint32_t), 1, file);
                fread(&block_align, sizeof(uint16_t), 1, file);
                fread(&bits_per_sample, sizeof(uint16_t), 1, file);
                fread(id, sizeof(uint8_t), 4, file);
                fread(&data_size, sizeof(uint32_t), 1, file);
                
                bufferData = (short*) malloc(data_size);
                fread(bufferData, sizeof(uint16_t), data_size/sizeof(uint16_t), file);
            }
            else {
                //cout<<"Error: RIFF file but not a wave file\n";
            }
        }
        else {
            //cout<<"Error: not a RIFF file\n";
        }
        
       return new AudioFile(channels, bits_per_sample, data_size, sample_rate, (void*) bufferData);
    }
    
    return NULL;
    //fclose(fp);
}
