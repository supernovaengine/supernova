
#include "JPEGReader.h"

#include "ColorType.h"
#include <stdlib.h>

#include "jpeglib.h"
#include "jerror.h"


TextureFile* JPEGReader::getRawImage(const char* relative_path, std::ifstream* ifile){

    unsigned long x, y;
    unsigned long data_size;
    unsigned char * jdata;
    
    int channels, bitssample;
    unsigned int type;
    unsigned char * rowptr[1];
    
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    
    info.err = jpeg_std_error(& err);
    jpeg_create_decompress(& info);

    std::size_t size;

    ifile->seekg(0, std::ios::end);
    size = ifile->tellg();
    ifile->seekg(0);

    buffer ibuf(size);

    ifile->read((char*)&ibuf[0], ibuf.size());

    jpeg_mem_src(&info, &ibuf[0], ibuf.size());

    jpeg_read_header(&info, TRUE);
    
    jpeg_start_decompress(&info);

    x = info.output_width;
    y = info.output_height;
    channels = info.num_components;
    bitssample = info.data_precision;

    type = S_COLOR_RGB;
    if(channels == 4) type = S_COLOR_RGB_ALPHA;
    
    data_size = x * y * channels;
    
    // read scanlines one at a time & put bytes in jdata[] array
    jdata = (unsigned char *)malloc(data_size);
    while (info.output_scanline < info.output_height)
    {
        rowptr[0] = (unsigned char *)jdata + channels * info.output_width * info.output_scanline;
        
        jpeg_read_scanlines(&info, rowptr, 1);
    }
    
    jpeg_finish_decompress(&info);
    
    TextureFile* textureFile  = new TextureFile((int)x, (int)y, (int)data_size, type, channels * bitssample, (void*)jdata);
    textureFile->flipVertical();
    
    return textureFile;

}
