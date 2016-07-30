#ifndef image_h
#define image_h

#include "File.h"
#include "PNGRead.h"
#include "RawImage.h"

class Image{

private:
    PNGRead pngRead;
    File file;
    RawImage rawImage;
public:
    Image();
    Image(const char* relative_path);
    virtual ~Image();

    void loadRawImage(const char* relative_path);
    void release_raw_image_data();

    RawImage getRawImage();

};


#endif /* image_h */
