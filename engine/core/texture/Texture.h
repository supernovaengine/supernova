#ifndef TEXTURE_H
#define TEXTURE_H

#include "render/TextureRender.h"
#include "texture/Framebuffer.h"
#include "texture/TextureData.h"
#include "pool/TexturePool.h"
#include <string>
#include <memory>

namespace Supernova{

    class Texture{
        private:
            std::shared_ptr<TexturePoolData> renderAndData = NULL;
            Framebuffer* framebuffer;
            unsigned long lastFramebufferVersion = 0;

            TextureType type;
            std::string id;

            std::string paths[6];
            TextureData data[6];

            int numFaces;

            bool loadFromPath;
            bool releaseDataAfterLoad;
            bool needLoad;

            //render properties
            TextureFilter minFilter;
            TextureFilter magFilter;
            TextureWrap wrapU;
            TextureWrap wrapV;

            // render callback clean function
            static void cleanupTexture(void* data);

        public:
            Texture();
            Texture(std::string path);
            Texture(TextureData data, std::string id);

            void setPath(std::string path);
            void setData(TextureData data, std::string id);
            void setCubePath(size_t index, std::string path);
            void setCubePaths(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down);
            void setFramebuffer(Framebuffer* framebuffer);

            Texture(const Texture& rhs);
            Texture& operator=(const Texture& rhs);

            bool operator == ( const Texture& rhs ) const;
            bool operator != ( const Texture& rhs ) const;

            virtual ~Texture();

            bool load();
            void destroy();

            TextureRender* getRender();
            std::string getPath(size_t index = 0);
            TextureData& getData(size_t index = 0);

            int getWidth();
            int getHeight();

            void setReleaseDataAfterLoad(bool releaseDataAfterLoad);
            bool isReleaseDataAfterLoad() const;

            void releaseData();

            bool empty();
            bool isFramebuffer();
            bool isFramebufferOutdated();

            void setMinFilter(TextureFilter filter);
            TextureFilter getMinFilter() const;

            void setMagFilter(TextureFilter filter);
            TextureFilter getMagFilter() const;

            void setWrapU(TextureWrap wrapU);
            TextureWrap getWrapU() const;

            void setWrapV(TextureWrap wrapV);
            TextureWrap getWrapV() const;
    };
}

#endif //TEXTURE_H