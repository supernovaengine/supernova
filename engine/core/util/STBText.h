//
// (c) 2024 Eduardo Doria.
//

#ifndef STBText_h
#define STBText_h

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "buffer/InterleavedBuffer.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#include "texture/Texture.h"

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

        TextureData* textureData;

        void tryFindBitmapSize(const stbtt_fontinfo *info, float scale);

    public:
        STBText();
        virtual ~STBText();

        float getAscent();
        float getDescent();
        float getLineGap();
        int getLineHeight();

        TextureData* load(std::string fontpath, unsigned int fontSize);
        void createText(std::string text, Buffer* buffer, std::vector<uint16_t>& indices, std::vector<Vector2>& charPositions,
                        int& width, int& height, bool fixedWidth, bool fixedHeight, bool multiline, bool invert);

        TextureData* getTextureData();
        
    };
    
}

#endif /* STBText_h */
