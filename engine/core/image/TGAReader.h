
#ifndef TGAReader_h
#define TGAReader_h

#include <vector>
#include "ImageReader.h"

typedef union PixelInfo
{
    std::uint32_t Colour;
    struct
    {
        std::uint8_t B, G, R, A;
    };
} *PPixelInfo;


class TGAReader: public ImageReader{
    
private:

    bool ImageCompressed;
    std::uint32_t width, height, size, BitsPerPixel;

public:
    TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile);
    
    std::uint32_t GetWidth() const {return this->width;}
    std::uint32_t GetHeight() const {return this->height;}
    bool HasAlphaChannel() {return BitsPerPixel == 32;}    
};
#endif /* TGAReader_h */
