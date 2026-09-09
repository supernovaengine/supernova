// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef FONTPOOL_H
#define FONTPOOL_H

#include "util/STBText.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace doriax{

    typedef std::map< std::string, std::shared_ptr<STBText> > fonts_t;

    class DORIAX_API FontPool{
    private:
        static fonts_t& getMap();

    public:
        static std::shared_ptr<STBText> get(const std::string& id);
        static std::shared_ptr<STBText> get(const std::string& id, const std::string& fontpath, const std::vector<std::string>& fallbackPaths, unsigned int fontSize);
        static void remove(const std::string& id);

        // necessary for engine shutdown
        static void clear();
        // removes only entries no one else references (safe while other scenes are alive)
        static void clearUnused();

    };
}

#endif /* FONTPOOL_H */
