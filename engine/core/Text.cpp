#include "Text.h"

#include "stb_truetype.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"
#include "platform/Log.h"
#include "image/ColorType.h"
#include "FileData.h"

Text::Text(): Mesh2D() {
    primitiveMode = S_TRIANGLES;
}

Text::Text(std::string text): Mesh2D() {
    primitiveMode = S_TRIANGLES;
    setTexture(text);
}

Text::~Text() {
}

void Text::createVertices(){
    vertices.clear();
    vertices.push_back(Vector3(0, 0, 0));
    vertices.push_back(Vector3(width, 0, 0));
    vertices.push_back(Vector3(width,  height, 0));
    vertices.push_back(Vector3(0,  height, 0));

    texcoords.clear();
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    static const unsigned int indices_array[] = {
        0,  1,  2,
        0,  2,  3
    };

    std::vector<unsigned int> indices;
    indices.assign(indices_array, std::end(indices_array));
    submeshes[0]->setIndices(indices);

    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
}


bool Text::load(){

    const unsigned int size = 80;
    const unsigned int atlasWidth = 512;
    const unsigned int atlasHeight = 512;
    const unsigned int oversampleX = 2;
    const unsigned int oversampleY = 2;
    const unsigned int firstChar = ' ';
    const unsigned int charCount = '~' - ' ';


    //if (!loaded && this->width == 0 && this->height == 0){
        //TextureManager::loadTexture(submeshes[0]->getTextures()[0]);
        //this->width = atlasWidth;
        //this->height = atlasHeight;
    //}

    FileData* fontData = new FileData();
    fontData->open("arial.ttf");
    
    unsigned char* atlasData = new unsigned char[atlasWidth * atlasHeight];
    stbtt_packedchar* charInfo = new stbtt_packedchar[charCount];
    
    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, atlasData, atlasWidth, atlasHeight, 0, 1, nullptr))
        Log::Error(LOG_TAG, "Failed to initialize font");


    stbtt_PackSetOversampling(&context, oversampleX, oversampleY);
    if (!stbtt_PackFontRange(&context, fontData->getMemPtr(), 0, size, firstChar, charCount, charInfo))
        Log::Error(LOG_TAG, "Failed to pack font");

    stbtt_PackEnd(&context);


    TextureFile* textureFile  = new TextureFile(atlasWidth, atlasHeight, (int)(atlasWidth * atlasHeight * sizeof(unsigned char)), S_COLOR_GRAY_ALPHA, 8, (void*)atlasData);
    textureFile->flipVertical();
    TextureManager::loadTexture(textureFile, "font");

    setTexture("font");

    createVertices();
/*
    float offsetX = 0;
    float offsetY = 0;
    const char text = 'A';
    stbtt_aligned_quad quad;
    stbtt_GetPackedQuad(charInfo, atlasWidth, atlasHeight, text - firstChar, &offsetX,
                        &offsetY, &quad, 1);
    texcoords.clear();
    texcoords.push_back(Vector2(quad.s0, quad.t0));
    texcoords.push_back(Vector2(quad.s1, quad.t0));
    texcoords.push_back(Vector2(quad.s1, quad.t1));
    texcoords.push_back(Vector2(quad.s0, quad.t1));
*/
    /*
    float offsetX = 0;
    float offsetY = 0;
    const char *text = "A";

    vertices.clear();
    texcoords.clear();

    while (*text) {
        if (*text >= firstChar && *text < firstChar + charCount) {
            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(charInfo, atlasWidth, atlasHeight, *text - firstChar, &offsetX,
                                &offsetY, &quad, 1);
            float auxt0 = quad.t0;
            quad.t0 = 1 - quad.t1;
            quad.t1 = 1 - auxt0;

            vertices.push_back(Vector3(quad.x0, quad.y0, 0));
            vertices.push_back(Vector3(quad.x1, quad.y0, 0));
            vertices.push_back(Vector3(quad.x1, quad.y1, 0));
            vertices.push_back(Vector3(quad.x0, quad.y1, 0));

            texcoords.push_back(Vector2(quad.s0, quad.t0));
            texcoords.push_back(Vector2(quad.s1, quad.t0));
            texcoords.push_back(Vector2(quad.s1, quad.t1));
            texcoords.push_back(Vector2(quad.s0, quad.t1));

        }
        ++text;
    }
*/

        
        /*
    unsigned char ttf_buffer[1<<20];
    unsigned char temp_bitmap[512*512];
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    
    fread(ttf_buffer, 1, 1<<20, fopen("arial.ttf", "rb"));
    stbtt_BakeFontBitmap(ttf_buffer,0, 32.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
*/
    

    return Mesh2D::load();
}
