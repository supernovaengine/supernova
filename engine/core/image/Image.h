#ifndef image_h
#define image_h

#include "ImageRead.h"
#include "RawImage.h"

class Image{

private:
    ImageRead* imageRead;
    RawImage rawImage;
    
    ImageRead* getImageFormat(const char* relative_path, FILE* file_stream);
public:
    Image();
    Image(const char* relative_path);
    virtual ~Image();

    void loadRawImage(const char* relative_path);
    void releaseImage();

    RawImage getRawImage();

};


#endif /* image_h */
