//
// (c) 2023 Eduardo Doria.
//

#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H

#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <map>
#include <memory>

namespace Supernova{

    typedef std::map< std::string, std::shared_ptr<TextureRender> > textures_t;

    class TexturePool{
    private:
        static textures_t& getMap();

    public:
        static std::shared_ptr<TextureRender> get(std::string id);
        static std::shared_ptr<TextureRender> get(std::string id, TextureType type, TextureData data[6], TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV);
        static void remove(std::string id);

        // necessary for engine shutdown
        static void clear();

    };
}

#endif /* TEXTUREPOOL_H */
