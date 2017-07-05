#include "STBText.h"

#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"
#include "platform/Log.h"
#include "image/ColorType.h"
#include "FileData.h"
#include <codecvt>

using namespace Supernova;

STBText::STBText() {
    atlasWidth = 0;
    atlasHeight = 0;

    ascent = 0;
    descent = 0;
    lineGap = 0;
    lineHeight = 0;
}

STBText::~STBText() {
}

void STBText::tryFindBitmapSize(const stbtt_fontinfo *info, float scale){

    atlasWidth = 512;
    atlasHeight = 512;

    int x0; int y0; int x1; int y1;

    stbtt_GetFontBoundingBox(info, &x0, &y0, &x1, &y1);
    int gfh = (y1-y0) * scale;

    bool fitBitmap = false;
    while (!fitBitmap && atlasWidth <= atlasLimit){

        int xOffset = 0;
        int yOffset = 0;

        for (int i = firstChar; (i < lastChar && yOffset <= atlasHeight && xOffset <= atlasWidth); i++){
            stbtt_GetCodepointBox(info, i, &x0, &y0, &x1, &y1);
            int gw = (x1-x0) * scale;
            xOffset += gw+1;
            if (xOffset > atlasWidth){
                xOffset = 0;
                yOffset += gfh;
            }
        }

        if (yOffset > atlasHeight || xOffset > atlasWidth){
            atlasWidth = 2 * atlasWidth;
            atlasHeight = 2 * atlasHeight;
        }else {
            fitBitmap = true;
        }
    }
}

float STBText::getAscent(){
    return ascent;
}

float STBText::getDescent(){
    return descent;
}

float STBText::getLineGap(){
    return lineGap;
}

int STBText::getLineHeight(){
    return lineHeight;
}

bool STBText::load(const char* font, unsigned int fontSize){

    FileData* fontData = new FileData();
    fontData->open(font);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontData->getMemPtr(), 0)) {
        Log::Error(LOG_TAG, "Failed to initialize font");
        return false;
    }
    float scale = stbtt_ScaleForPixelHeight(&info, fontSize);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    this->ascent = ascent * scale;
    this->descent = descent * scale;
    this->lineGap = lineGap * scale;
    this->lineHeight = (ascent - descent + lineGap) * scale;

    tryFindBitmapSize(&info, scale);

    unsigned char *atlasData = NULL;
    charInfo = new stbtt_packedchar[charCount];

    stbtt_pack_context context;

    bool fitBitmap = false;
    while (!fitBitmap && atlasWidth <= atlasLimit) {
        if (atlasData) delete[] atlasData;
        atlasData = new unsigned char[atlasWidth * atlasHeight];

        if (!stbtt_PackBegin(&context, atlasData, atlasWidth, atlasHeight, 0, 1, nullptr)){
            Log::Error(LOG_TAG, "Failed to initialize font");
            return false;
        }

        //stbtt_PackSetOversampling(&context, oversampleX, oversampleY);
        if (!stbtt_PackFontRange(&context, fontData->getMemPtr(), 0, fontSize, firstChar, charCount, charInfo)){
            atlasWidth = atlasWidth * 2;
            atlasHeight = atlasHeight * 2;
        }else{
            fitBitmap = true;
        }
    }
    if (atlasWidth > atlasLimit){
        Log::Error(LOG_TAG, "Failed to pack font");
        return false;
    }
    stbtt_PackEnd(&context);

    unsigned int textureSize = atlasWidth * atlasHeight * sizeof(unsigned char);
    TextureData* textureData  = new TextureData(atlasWidth, atlasHeight, textureSize, S_COLOR_ALPHA, 8, (void*)atlasData);
    TextureManager::loadTexture(textureData, font + std::to_string('-') + std::to_string(fontSize));

    delete[] atlasData;

    delete fontData;

    return true;
}

void STBText::createText(std::string text, std::vector<Vector3>* vertices, std::vector<Vector3>* normals, std::vector<Vector2>* texcoords,
                         std::vector<unsigned int>* indices, int* width, int* height, bool userDefinedWidth, bool userDefinedHeight, bool multiline, bool invert){
    
    std::wstring_convert< std::codecvt_utf8_utf16<wchar_t> > convert;
    std::wstring utf16String = convert.from_bytes( text );
    
    float offsetX = 0;
    float offsetY = 0;

    if (multiline && userDefinedWidth){

        int lastSpace = 0;
        for (int i = 0; i < utf16String.size(); i++){
            int intchar = uint_least32_t(utf16String[i]);
            if (intchar == 32){ //space
                lastSpace = i;
            }
            if (intchar == 10){ //\n
                offsetX = 0;
            }
            if (intchar >= firstChar && intchar <= lastChar) {
                stbtt_aligned_quad quad;
                stbtt_GetPackedQuad(charInfo, atlasWidth, atlasHeight, intchar - firstChar, &offsetX, &offsetY, &quad, 1);
                
                if (offsetX > (*width)){
                    if (lastSpace > 0){
                        utf16String[lastSpace] = '\n';
                        i = lastSpace;
                        lastSpace = 0;
                    }else{
                        utf16String.insert(i, { '\n' });
                    }
                    offsetX = 0;
                }
            }
        }

        offsetX = 0;
        offsetY = 0;
    }

    int minX0 = 0, maxX1 = 0, minY0 = 0, maxY1 = 0;
    int ind = 0;
    int lineCount = 1;

    for (int i = 0; i < utf16String.size(); i++){
        int intchar = uint_least32_t(utf16String[i]);
        if (intchar == 10){ //\n
            offsetY += lineHeight;
            offsetX = 0;
            lineCount++;
        }
        if (intchar >= firstChar && intchar <= lastChar) {
            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(charInfo, atlasWidth, atlasHeight, intchar - firstChar, &offsetX, &offsetY, &quad, 1);
            
            if (invert) {
                float auxt0 = quad.t0;
                quad.t0 = quad.t1;
                quad.t1 = auxt0;

                float auxy0 = quad.y0;
                quad.y0 = -quad.y1;
                quad.y1 = -auxy0;
            }
            
            if (quad.x0 < minX0)
                minX0 = quad.x0;
            if (quad.y0 < minY0)
                minY0 = quad.y0;
            if (quad.x1 > maxX1)
                maxX1 = quad.x1;
            if (quad.y1 > maxY1)
                maxY1 = quad.y1;
            
            if ((!userDefinedWidth || offsetX <= *width) && (!userDefinedHeight || offsetY <= *height)){
                vertices->push_back(Vector3(quad.x0, quad.y0, 0));
                vertices->push_back(Vector3(quad.x1, quad.y0, 0));
                vertices->push_back(Vector3(quad.x1, quad.y1, 0));
                vertices->push_back(Vector3(quad.x0, quad.y1, 0));
                
                texcoords->push_back(Vector2(quad.s0, quad.t0));
                texcoords->push_back(Vector2(quad.s1, quad.t0));
                texcoords->push_back(Vector2(quad.s1, quad.t1));
                texcoords->push_back(Vector2(quad.s0, quad.t1));
                
                normals->push_back(Vector3(0.0f, 0.0f, 1.0f));
                normals->push_back(Vector3(0.0f, 0.0f, 1.0f));
                normals->push_back(Vector3(0.0f, 0.0f, 1.0f));
                normals->push_back(Vector3(0.0f, 0.0f, 1.0f));
                
                indices->push_back(ind);
                indices->push_back(ind+1);
                indices->push_back(ind+2);
                indices->push_back(ind);
                indices->push_back(ind+2);
                indices->push_back(ind+3);
                ind = ind + 4;
            }
            
        }
    }
    if (!userDefinedWidth)
        (*width) = maxX1 - minX0;
    if (!userDefinedHeight)
        (*height) = lineCount * lineHeight;

}
