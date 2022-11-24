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

        bool createTexture(
                std::string label, int width, int height, 
                ColorFormat colorFormat, TextureType type, int numFaces, void* data[6], size_t size[6],
                TextureFilter minFilter, TextureFilter magFilter);

        bool createFramebufferTexture(
                TextureType type, bool depth, bool shadowMap, int width, int height, 
                TextureFilter minFilter, TextureFilter magFilter);

        void destroyTexture();
    };
}


#endif /* TextureRender_h */