//
// (c) 2024 Eduardo Doria.
//

#ifndef FONTPOOL_H
#define FONTPOOL_H

#include "util/STBText.h"
#include <map>
#include <memory>

namespace Supernova{

    typedef std::map< std::string, std::shared_ptr<STBText> > fonts_t;

    class FontPool{
    private:
        static fonts_t& getMap();

    public:
        static std::shared_ptr<STBText> get(std::string id);
        static std::shared_ptr<STBText> get(std::string id, std::string fontpath, unsigned int fontSize);
        static void remove(std::string id);

        // necessary for engine shutdown
        static void clear();

    };
}

#endif /* FONTPOOL_H */
