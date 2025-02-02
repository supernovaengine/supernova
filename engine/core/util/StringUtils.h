//
// (c) 2024 Eduardo Doria.
//

#ifndef StringUtils_h
#define StringUtils_h

#include "Export.h"
#include <string>

namespace Supernova{
    class SUPERNOVA_API StringUtils{
    public:
        static std::string toUTF8(wchar_t codepoint);
    };
}

#endif /* StringUtils_h */
