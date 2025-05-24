//
// (c) 2024 Eduardo Doria.
//

#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H

#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <map>
#include <memory>
#include <array>

namespace Supernova{

    typedef std::map< std::string, std::shared_ptr<TextureRender> > textures_t;

    class SUPERNOVA_API TexturePool{
    private:
        static textures_t& getMap();

    public:
        static std::shared_ptr<TextureRender> get(const std::string& id);
        static std::shared_ptr<TextureRender> get(const std::string& id, TextureType type, std::array<TextureData,6> data, TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV);
        static void remove(const std::string& id);

        // necessary for engine shutdown
        static void clear();

    };
}

#endif /* TEXTUREPOOL_H */
