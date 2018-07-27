#ifndef gles2texture_h
#define gles2texture_h

#define MAXTEXTUREID_GLES2 4096

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

        virtual bool loadTexture(TextureData* texturedata);
        virtual bool loadTextureCube(std::vector<TextureData*> texturesdata);
        virtual bool loadTextureFrame(int width, int height, bool depthframe);
        virtual bool loadTextureFrameCube(int width, int height);
        virtual void deleteTexture();
        
        virtual void initTextureFrame();
        virtual void initTextureFrame(int cubeFace);
        virtual void endTextureFrame();
        
        void setTexture(GLuint texture);
        GLuint getTexture();
        GLenum getTextureType();

    };
    
}

#endif /* gles2_texture_h */
