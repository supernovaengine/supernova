//
// (c) 2024 Eduardo Doria.
//

#ifndef TEXTUREDATAPOOL_H
#define TEXTUREDATAPOOL_H

#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <map>
#include <memory>
#include <array>

namespace Supernova{

    typedef std::map< std::string, std::shared_ptr<std::array<TextureData,6>> > texturesdata_t;

    class SUPERNOVA_API TextureDataPool{
    private:
        static texturesdata_t& getMap();

    public:
        static std::shared_ptr<std::array<TextureData,6>> get(const std::string& id);
        static std::shared_ptr<std::array<TextureData,6>> get(const std::string& id, std::array<TextureData,6> data);
        static void remove(const std::string& id);

        // necessary for engine shutdown
        static void clear();

    };
}

#endif /* TEXTUREDATAPOOL_H */
