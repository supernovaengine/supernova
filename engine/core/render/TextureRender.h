#ifndef TextureRender_h
#define TextureRender_h

#include "Render.h"
#include "sokol/SokolTexture.h"

namespace Supernova{
    class TextureRender{

    public:
        //***Backend***
        SokolTexture backend;
        //***

        TextureRender();
        TextureRender(const TextureRender& rhs);
        TextureRender& operator=(const TextureRender& rhs);

        virtual ~TextureRender();

        bool createTexture(std::string label, int width, int height, ColorFormat colorFormat, TextureType type, int numFaces, TextureDataSize* texData);
        bool createFramebufferTexture(TextureType type, bool depth, bool shadowMap, int width, int height);

        void destroyTexture();
    };
}


#endif /* TextureRender_h */