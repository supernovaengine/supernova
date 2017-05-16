#include "Text.h"

#include "stb_truetype.h"


#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"
#include "platform/Log.h"

Text::Text(): Mesh2D() {
    primitiveMode = S_TRIANGLES;
/*
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        Log::Error(LOG_TAG, "FREETYPE: Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, "teste.ttf", 0, &face))
        Log::Error(LOG_TAG, "FREETYPE: Failed to load font");
*/
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

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
}


bool Text::load(){
    /*
    if (submeshes[0]->getTextures().size() > 0 && !loaded && this->width == 0 && this->height == 0){
        TextureManager::loadTexture(submeshes[0]->getTextures()[0]);
        this->width = TextureManager::getTextureWidth(submeshes[0]->getTextures()[0]);
        this->height = TextureManager::getTextureHeight(submeshes[0]->getTextures()[0]);
    }
*/
    
    int atlasWidth = 512;
    int atlasHeight = 512;
    const int oversampleX = 2;
    const int oversampleY = 2;
    const int firstChar = ' ';
    const int charCount = '~' - ' ';
    
    uint8_t* atlasData = new uint8_t[atlasWidth * atlasHeight];
    stbtt_packedchar* charInfo = new stbtt_packedchar[charCount];
    
    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, atlasData, atlasWidth, atlasHeight, 0, 1, nullptr))
        Log::Error(LOG_TAG, "Failed to initialize font");
    
        
        
        /*
    unsigned char ttf_buffer[1<<20];
    unsigned char temp_bitmap[512*512];
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    
    fread(ttf_buffer, 1, 1<<20, fopen("arial.ttf", "rb"));
    stbtt_BakeFontBitmap(ttf_buffer,0, 32.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
*/
    
    //createVertices();
    //return Mesh2D::load();
    return 0;
}
