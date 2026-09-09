// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef STBText_h
#define STBText_h

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "buffer/InterleavedBuffer.h"
#include "io/Data.h"
#include "util/CharExtent.h"
#include "stb_truetype.h"
#include "texture/Texture.h"

//HarfBuzz objects, kept opaque so its headers stay out of the engine API
struct hb_blob_t;
struct hb_face_t;
struct hb_font_t;

namespace doriax {

    //glyph atlas of a font in a single size, plus the fonts it falls back to. The whole
    //chain shares one atlas, so mixed scripts stay a single texture
    class DORIAX_API STBText {

    private:

        struct FontGlyph {
            //quad corners relative to the pen position, y-down from the baseline
            float xoff;
            float yoff;
            float xoff2;
            float yoff2;

            float s0;
            float t0;
            float s1;
            float t1;

            float xadvance;
        };

        //a row of the atlas, shared by glyphs of similar height
        struct Shelf {
            int x;
            int y;
            int h;
        };

        //a glyph the shaper picked, already in visual order inside its line
        struct ShapedGlyph {
            unsigned int face;
            bool rtl;
            uint32_t glyphIndex;
            float xOffset;
            float yOffset;
            float xAdvance;
            //codepoint it came from, shared when a ligature covers several
            size_t cluster;
            unsigned int line;
        };

        //held by pointer, stb_truetype and HarfBuzz both keep references into the data
        struct FontFace {
            Data data;
            stbtt_fontinfo info;
            //own scale, faces of different unitsPerEm still render the same size
            float scale = 0.0f;

            hb_blob_t* blob = NULL;
            hb_face_t* face = NULL;
            hb_font_t* font = NULL;

            ~FontFace();
        };

        //max texture size guaranteed by GLES3 and WebGL2
        const unsigned int atlasLimit = 4096;
        const int atlasPadding = 1;

        std::vector<std::unique_ptr<FontFace>> faces;
        bool fontLoaded;

        //key is the face index and the glyph index of that face packed together
        std::unordered_map<uint64_t, FontGlyph> glyphMap;
        std::unordered_map<uint32_t, uint64_t> codepointMap;

        std::vector<unsigned char> atlasPixels;
        std::vector<Shelf> shelves;
        unsigned int atlasWidth;
        unsigned int atlasHeight;

        //buffers of previous atlas sizes, TextureData points to them without owning
        std::vector<std::vector<unsigned char>> retiredAtlases;

        //bumped on every atlas change, texts built with an older one are outdated
        unsigned long atlasVersion;

        float ascent;
        float descent;
        float lineGap;
        int lineHeight;

        TextureData* textureData;

        static uint64_t glyphKey(unsigned int face, uint32_t glyphIndex);

        bool initFace(FontFace* face, unsigned int fontSize, const std::string& label);
        bool addFace(const std::string& fontpath, unsigned int fontSize);
        bool addMemoryFace(unsigned char* data, unsigned int length, unsigned int fontSize, const std::string& label);
        //the fonts compiled into the engine, Latin then Arabic
        void addBuiltInFaces(unsigned int fontSize);
        //first face covering the codepoint, keeping the one in use so a shared
        //character like a space does not split a run
        unsigned int selectFace(uint32_t codepoint, unsigned int current) const;

        void shapeRun(const std::vector<uint32_t>& codepoints, size_t offset, size_t length, bool rtl, unsigned int face, uint32_t script, unsigned int line, std::vector<ShapedGlyph>& shaped);
        //splits the bidi run again, one piece per face and per script
        void shapeRunItems(const std::vector<uint32_t>& codepoints, size_t offset, size_t length, bool rtl, unsigned int line, std::vector<ShapedGlyph>& shaped);
        void shapeRange(const std::vector<uint32_t>& codepoints, size_t offset, size_t length, unsigned int line, std::vector<ShapedGlyph>& shaped);
        void measureAdvances(const std::vector<uint32_t>& codepoints, size_t offset, size_t length, std::vector<float>& advances);
        //shapes a candidate line alone and returns the width it really takes
        float shapeLineInto(const std::vector<uint32_t>& codepoints, size_t offset, size_t length, unsigned int line, std::vector<ShapedGlyph>& lineGlyphs);
        //splits on newlines, wraps to width, and shapes each resulting line
        unsigned int shapeLines(const std::vector<uint32_t>& codepoints, unsigned int width, bool fixedWidth, bool multiline, std::vector<ShapedGlyph>& shaped);

        void resetAtlas(unsigned int width, unsigned int height);
        bool packRect(int width, int height, int& outX, int& outY);
        bool growAtlas();
        void rasterizeGlyph(unsigned int face, uint32_t glyphIndex, FontGlyph& glyph, int x, int y);
        const FontGlyph* getGlyph(unsigned int face, uint32_t glyphIndex);
        const FontGlyph* getGlyphForCodepoint(uint32_t codepoint);
        void refreshTextureData();

    public:
        STBText();
        virtual ~STBText();

        //unique_ptr faces are not copyable; MSVC instantiates the copy path unless these are deleted
        STBText(const STBText&) = delete;
        STBText& operator=(const STBText&) = delete;

        float getAscent();
        float getDescent();
        float getLineGap();
        int getLineHeight();
        float getCharWidth(uint32_t codepoint);

        unsigned long getAtlasVersion() const;

        TextureData* load(const std::string& fontpath, const std::vector<std::string>& fallbackPaths, unsigned int fontSize);
        void createText(const std::string& text, Buffer* buffer, std::vector<uint16_t>& indices, std::vector<Vector2>& charPositions,
                        std::vector<CharExtent>& charExtents,
                        unsigned int& width, unsigned int& height, bool fixedWidth, bool fixedHeight, bool multiline, bool invert);

        TextureData* getTextureData();
        
    };
    
}

#endif /* STBText_h */
