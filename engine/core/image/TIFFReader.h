#ifndef TIFFReader_h
#define TIFFReader_h

#include "ImageReader.h"

class TIFFReader: public ImageReader{

public:

    TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile);

};

#endif /* TIFFReader_h */
