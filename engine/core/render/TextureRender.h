
#ifndef TextureRender_h
#define TextureRender_h

#include "image/TextureData.h"
#include <vector>

namespace Supernova {

    class TextureRender {
        
    public:
        
        inline virtual ~TextureRender(){}
        
        virtual void loadTexture(TextureData* texturedata) = 0;
        virtual void loadTextureCube(std::vector<TextureData*> texturesdata) = 0;
        virtual void deleteTexture() = 0;
        
    };
    
}

#endif /* TextureRender_h */
