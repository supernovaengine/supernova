
#ifndef STBText_h
#define STBText_h

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "util/AttributeBuffer.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#include "Texture.h"

namespace Supernova {

    class STBText {

    private:

        const unsigned int atlasLimit = 32768;
        const unsigned int firstChar = 32;
        const unsigned int lastChar = 255;
        const unsigned int charCount = lastChar - firstChar;
        
        stbtt_packedchar *charInfo;

        unsigned int atlasWidth;
        unsigned int atlasHeight;

        float ascent;
        float descent;
        float lineGap;
        int lineHeight;

        void tryFindBitmapSize(const stbtt_fontinfo *info, float scale);

    public:
        STBText();
        virtual ~STBText();

        float getAscent();
        float getDescent();
        float getLineGap();
        int getLineHeight();

        bool load(const char* font, unsigned int fontSize, Texture* texture);
        void createText(std::string text, AttributeBuffer& buffer, std::vector<unsigned int>* indices,
                        int* width, int* height, bool userDefinedWidth, bool userDefinedHeight, bool multiline, bool invert);
        
    };
    
}

#endif /* Text_h */
