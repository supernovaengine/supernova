#include "TextureLoader.h"

#include "STBImageReader.h"
#include "platform/Log.h"
#include <assert.h>

TextureLoader::TextureLoader() {
    rawImage = NULL;
}

TextureLoader::TextureLoader(std::string relative_path){
    loadRawImage(relative_path);
}

TextureLoader::~TextureLoader() {
    delete rawImage;
}

void TextureLoader::loadRawImage(std::string relative_path) {
    
    assert(relative_path != "");
    
    //Texture data orientation is top left (first loaded bytes as lower-valued V coordinates)
    
    FileData* filedata = new FileData();
    filedata->open(relative_path.c_str());
    /*
    if (!ifs){
        Log::Error(LOG_TAG, "Can`t load texture file: %s", relative_path.c_str());
    }
    assert(ifs);
    */
    
    ImageReader* imageReader = getImageFormat(filedata);
    
    if (imageReader==NULL){
        Log::Error(LOG_TAG, "Not supported image format from: %s", relative_path.c_str());
    }
    assert(imageReader!=NULL);

    filedata->seek(0);
    
    rawImage = imageReader->getRawImage(filedata);
    
    delete filedata;
    delete imageReader;

}

ImageReader* TextureLoader::getImageFormat(FileData* filedata){
    
    std::uint8_t DeCompressed[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    std::uint8_t IsCompressed[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    
    filedata->seek(0);
    unsigned char* fileInfo = (unsigned char*) malloc(12 * sizeof(unsigned char));
    filedata->read(fileInfo, 12);
    
    unsigned int magic = (fileInfo[0] << 24) | (fileInfo[1] << 16) | (fileInfo[2] << 8) | fileInfo[3];
    
    if (magic == 0x89504E47U) { //PNG
        return new STBImageReader();
    } else if (magic >= 0xFFD8FF00U && magic <= 0xFFD8FFFFU) { //JPEG
        return new STBImageReader(); //JPEG
    //} else if (magic == 0x49492A00 || magic == 0x4D4D002A) { //TIFF
    //    return new TIFFReader();
    } else if (magic == 0x47494638) { //GIF
        return new STBImageReader();
    } else if (fileInfo[0] == 'B' && fileInfo[1] == 'M') { //BMP
        return new STBImageReader();
    } else if (!std::memcmp(DeCompressed, &fileInfo[0], sizeof(DeCompressed)) ||
               !std::memcmp(IsCompressed, &fileInfo[0], sizeof(IsCompressed))){ //TGA
        return new STBImageReader();
    }
    
    return NULL;
}

TextureFile* TextureLoader::getRawImage(){
    return rawImage;
}
