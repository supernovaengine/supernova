#include "TextureLoader.h"

#include "PNGReader.h"
#include "JPEGReader.h"
#include "TGAReader.h"
#include "TIFFReader.h"
#include "BMPReader.h"
#include "platform/Log.h"
#include <assert.h>

TextureLoader::TextureLoader() {
    rawImage = NULL;
}

TextureLoader::TextureLoader(const char* relative_path){
    loadRawImage(relative_path);
}

TextureLoader::~TextureLoader() {
    delete rawImage;
}

void TextureLoader::loadRawImage(const char* relative_path) {
    assert(relative_path != NULL);
    
    std::ifstream ifs(relative_path, std::ios::binary);
    if (!ifs){
        Log::Error(LOG_TAG, "Can`t load texture file: %s", relative_path);
    }
    assert(ifs);
    
    ImageReader* imageReader = getImageFormat(relative_path, &ifs);
    
    if (imageReader==NULL){
        Log::Error(LOG_TAG, "Not supported image format from: %s", relative_path);
    }
    assert(imageReader!=NULL);

    ifs.seekg(0, std::ios::beg);
    
    rawImage = imageReader->getRawImage(relative_path, &ifs);
    
    ifs.close();
    delete imageReader;

}

ImageReader* TextureLoader::getImageFormat(const char* relative_path, std::ifstream* ifile){
    
    
    static std::uint8_t DeCompressed[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    static std::uint8_t IsCompressed[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    
    ifile->seekg(0, std::ios::end);
    std::size_t Length = ifile->tellg();
    ifile->seekg(0, std::ios::beg);
    
    std::vector<std::uint8_t> fileInfo(Length);
    ifile->read(reinterpret_cast<char*>(fileInfo.data()), 12);
    
    unsigned int magic = (fileInfo[0] << 24) | (fileInfo[1] << 16) | (fileInfo[2] << 8) | fileInfo[3];
    
    if (magic == 0x89504E47U) {
        return new PNGReader();
    } else if (magic >= 0xFFD8FF00U && magic <= 0xFFD8FFFFU) {
        return new JPEGReader();
    } else if (magic == 0x49492A00 || magic == 0x4D4D002A) {
        return new TIFFReader();
    //} else if (magic == 0x47494638) {
        // format GIF;
    } else if (fileInfo[0] == 'B' && fileInfo[1] == 'M') {
        return new BMPReader();
    } else if (!std::memcmp(DeCompressed, &fileInfo.front(), sizeof(DeCompressed)) ||
               !std::memcmp(IsCompressed, &fileInfo.front(), sizeof(IsCompressed))){
        return new TGAReader();
    }

    return NULL;
}


TextureFile* TextureLoader::getRawImage(){
    return rawImage;
}
