//
// (c) 2024 Eduardo Doria.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "render/FramebufferRender.h"

namespace Supernova{

    class SUPERNOVA_API Framebuffer{

        private:
            FramebufferRender render;
            int width;
            int height;
            TextureFilter minFilter;
            TextureFilter magFilter;
            TextureWrap wrapU;
            TextureWrap wrapV;

            unsigned long version; // increment every creation

        public:
            Framebuffer();
            virtual ~Framebuffer();

            Framebuffer(const Framebuffer& rhs);
            Framebuffer& operator=(const Framebuffer& rhs);

            void create();
            void destroy();
            bool isCreated();

            FramebufferRender& getRender();
            unsigned long getVersion();

            void setWidth(int width);
            int getWidth() const;

            void setHeight(int height);
            int getHeight() const;

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

#endif //FRAMEBUFFER_H