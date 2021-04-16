//
// (c) 2021 Eduardo Doria.
//

#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H

#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <unordered_map>
#include <memory>

namespace Supernova{
    typedef std::unordered_map< std::string, std::shared_ptr<TextureRender> > textures_t;

    class TexturePool{
    private:
        static textures_t& getMap();

    public:
        static std::shared_ptr<TextureRender> get(std::string id);
        static std::shared_ptr<TextureRender> get(std::string id, TextureType type, TextureData* data);
        static void remove(std::string id);

    };
}

#endif /* TEXTUREPOOL_H */
