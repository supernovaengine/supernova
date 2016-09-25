#ifndef textureloader_h
#define textureloader_h

#include "ImageReader.h"
#include "Texture.h"

class TextureLoader{

private:
    TextureFile* rawImage;
    
    ImageReader* getImageFormat(const char* relative_path, std::ifstream* ifile);
public:
    TextureLoader();
    TextureLoader(const char* relative_path);
    virtual ~TextureLoader();

    void loadRawImage(const char* relative_path);

    TextureFile* getRawImage();

};


#endif /* textureloader_h */
