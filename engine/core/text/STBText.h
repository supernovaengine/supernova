
#ifndef STBText_h
#define STBText_h

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"

namespace Supernova {

    class STBText {

    private:

        const unsigned int atlasLimit = 32768;
        const unsigned int oversampleX = 2;
        const unsigned int oversampleY = 2;
        const unsigned int firstChar = 32;
        const unsigned int lastChar = 255;
        const unsigned int charCount = lastChar - firstChar;
        
        stbtt_packedchar *charInfo;

        unsigned int atlasWidth;
        unsigned int atlasHeight;

    public:
        STBText();
        virtual ~STBText();

        bool load(const char* font, unsigned int fontSize);
        void createText(std::string text, std::vector<Vector3>* vertices, std::vector<Vector3>* normals, std::vector<Vector2>* texcoords, std::vector<unsigned int>* indices, int* width, int* height, bool invert);
        
    };
    
}

#endif /* Text_h */
