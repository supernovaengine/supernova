#ifndef STBImageReader_h
#define STBImageReader_h

#include "ImageReader.h"

class STBImageReader: public ImageReader{
    
public:
    
    TextureFile* getRawImage(FileData* filedata);
    
};

#endif /* STBImageReader_h */
