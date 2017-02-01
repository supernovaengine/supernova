
#ifndef TextureRender_h
#define TextureRender_h

#include "image/TextureFile.h"
#include <vector>

class TextureRender {
    
public:
    
    inline virtual ~TextureRender(){}
    
    virtual void loadTexture(TextureFile* texturefile) = 0;
    virtual void loadTextureCube(std::vector<TextureFile*> texturefiles) = 0;
    virtual void deleteTexture() = 0;
    
};

#endif /* TextureRender_h */
