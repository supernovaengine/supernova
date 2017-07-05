#ifndef textureloader_h
#define textureloader_h

#include "ImageReader.h"

namespace Supernova {

    class TextureLoader{

    private:
        TextureData* rawImage;
        
        ImageReader* getImageFormat(FileData* filedata);
    public:
        TextureLoader();
        TextureLoader(std::string relative_path);
        virtual ~TextureLoader();

        void loadRawImage(std::string relative_path);

        TextureData* getRawImage();

    };
    
}


#endif /* textureloader_h */
