#ifndef textureloader_h
#define textureloader_h

#include "ImageReader.h"

class TextureLoader{

private:
    TextureFile* rawImage;
    
    ImageReader* getImageFormat(std::string relative_path, std::ifstream* ifile);
public:
    TextureLoader();
    TextureLoader(std::string relative_path);
    virtual ~TextureLoader();

    void loadRawImage(std::string relative_path);

    TextureFile* getRawImage();

};


#endif /* textureloader_h */
