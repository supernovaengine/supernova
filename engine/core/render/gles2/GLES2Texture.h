#ifndef gles2texture_h
#define gles2texture_h

#include "GLES2Header.h"
#include "render/TextureRender.h"

namespace Supernova {

    class GLES2Texture : public TextureRender{

    private:
        GLuint gTexture;
        GLenum textureType;
        GLuint frameBuffer;
        
        GLenum getGlColorFormat(const int color_format);
        void assignTexture(const GLenum target, const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);

    public:
        
        GLES2Texture();

        virtual void loadTexture(TextureData* texturedata);
        virtual void loadTextureCube(std::vector<TextureData*> texturesdata);
        virtual void loadTextureFrame(int width, int height);
        virtual void deleteTexture();
        
        virtual void initTextureFrame();
        virtual void endTextureFrame();
        
        void setTexture(GLuint texture);
        GLuint getTexture();
        GLenum getTextureType();

    };
    
}

#endif /* gles2_texture_h */
