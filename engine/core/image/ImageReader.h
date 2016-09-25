
#ifndef ImageReader_h
#define ImageReader_h

#include "TextureFile.h"
#include <fstream>

class ImageReader{
    
protected:
    void flipVertical(unsigned char* inbuf, int width, int height, int bitsPerPixel);

public:

    virtual TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile) = 0;

    virtual ~ImageReader();

};


#endif /* ImageRead_h */
