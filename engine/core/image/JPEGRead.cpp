
#include "JPEGRead.h"

#include "ColorType.h"
#include <stdlib.h>

#include "jpeglib.h"
#include "jerror.h"


RawImage JPEGRead::getRawImage(FILE* file){

    unsigned long x, y;
    unsigned long data_size;
    unsigned char * jdata;
    
    int channels;
    unsigned int type;
    unsigned char * rowptr[1];
    
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    
    info.err = jpeg_std_error(& err);
    jpeg_create_decompress(& info);

    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);
    
    jpeg_start_decompress(&info);

    x = info.output_width;
    y = info.output_height;
    channels = info.num_components;

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
    
    return RawImage((int)x, (int)y, (int)data_size, type, (void*)jdata);

}