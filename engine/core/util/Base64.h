//
// (c) 2022 Eduardo Doria.
// Based on LihO implementation:
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
//

#ifndef BASE64_H
#define BASE64_H

#include <vector>
#include <string>

namespace Supernova {
    class Base64 {
    private:
        static const std::string base64_chars;
        static bool is_base64(unsigned char c);

    public:
        static std::string encode(const unsigned char* buf, unsigned int bufLen);
        static std::vector<unsigned char> decode(std::string const& encoded_string);
    };
}


#endif //BASE64_H
