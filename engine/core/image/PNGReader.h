
#ifndef pngreader_h
#define pngreader_h

#include "png.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ImageReader.h"


class PNGReader: public ImageReader{

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
        const int bit_depth;
        const int channels;
    } PngInfo;

    static void readPngDataCallback( png_structp png_ptr, png_bytep out, png_size_t count );
    static PngInfo readAndUpdateInfo(const png_structp png_ptr, const png_infop info_ptr);
    static DataHandle readEntirePngImage(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
    static int getColorFormat(const int png_color_format);

public:

    TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile);

};

#endif /* pngreader_h */
