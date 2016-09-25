

#ifndef JPEGReader_h
#define JPEGReader_h

#include "ImageReader.h"
#include <vector>

typedef std::vector<unsigned char> buffer;

class JPEGReader: public ImageReader{
    
public:
    
    TextureFile* getRawImage(const char* relative_path, std::ifstream* ifile);
    
};

#endif /* JPEGReader_h */
