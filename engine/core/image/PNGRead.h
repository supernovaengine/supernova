
#ifndef pngread_h
#define pngread_h

#include "png.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ImageRead.h"


class PNGRead: public ImageRead{

private:

    typedef struct {
        const png_byte* data;
        const png_size_t size;
    } DataHandle;

    typedef struct {
        const DataHandle data;
        png_size_t offset;
    } ReadDataHandle;

    typedef struct {
        const png_uint_32 width;
        const png_uint_32 height;
        const int color_type;
    } PngInfo;

    static void readPngDataCallback(png_structp png_ptr, png_byte* png_data, png_size_t read_length);
    static PngInfo readAndUpdateInfo(const png_structp png_ptr, const png_infop info_ptr);
    static DataHandle readEntirePngImage(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
    static int getColorFormat(const int png_color_format);

public:

    RawImage getRawImage(FILE* file);

};

#endif /* pngread_h */
