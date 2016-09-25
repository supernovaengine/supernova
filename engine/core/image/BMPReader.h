
#ifndef BMPReader_h
#define BMPReader_h

#include "ImageReader.h"

class BMPReader: public ImageReader{
private:
    std::uint32_t width, height, size, BitsPerPixel;
    
public:
    TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile);
    
    std::uint32_t GetWidth() const {return this->width;}
    std::uint32_t GetHeight() const {return this->height;}
    
};

#endif /* BMPReader_h */
