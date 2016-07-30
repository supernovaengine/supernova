#include "Image.h"
#include <assert.h>

Image::Image() {
    //rawImage = NULL;
}

Image::Image(const char* relative_path){
    loadRawImage(relative_path);
}

Image::~Image() {
    rawImage.releaseImageData();
}

void Image::loadRawImage(const char* relative_path) {
    assert(relative_path != NULL);

    file.loadFile(relative_path);
    rawImage = pngRead.getRawImage(file.getData(), (int)file.getDataLength());
}


RawImage Image::getRawImage(){
    return rawImage;
}
