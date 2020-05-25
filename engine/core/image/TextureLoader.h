#ifndef textureloader_h
#define textureloader_h

#include "ImageReader.h"

namespace Supernova {

    class TextureLoader{

    private:
        
        ImageReader* getImageFormat(Data* filedata);
    public:
        TextureLoader();
        virtual ~TextureLoader();

        TextureData* loadTextureData(std::string relative_path);

    };
    
}


#endif /* textureloader_h */
