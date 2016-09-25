
#include "TGAReader.h"

#include "ColorType.h"
#include "platform/Log.h"


TextureFile* TGAReader::getRawImage(const char* relative_path, std::ifstream* ifile){
    
    char* imageData;

    std::uint8_t Header[18] = {0};
    static std::uint8_t DeCompressed[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    static std::uint8_t IsCompressed[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

    ifile->read(reinterpret_cast<char*>(&Header), sizeof(Header));

    if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed)))
    {
        BitsPerPixel = Header[16];
        width  = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size  = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            Log::Error(LOG_TAG, "Invalid File Format. Required: 24 or 32 Bit Image: %s", relative_path);
            return NULL;
        }

        imageData = new char[size];
        ImageCompressed = false;

        ifile->read(imageData, size);
    }
    else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed)))
    {
        BitsPerPixel = Header[16];
        width  = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size  = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            Log::Error(LOG_TAG, "Invalid File Format. Required: 24 or 32 Bit Image: %s", relative_path);
            return NULL;
        }

        PixelInfo Pixel = {0};
        int CurrentByte = 0;
        std::size_t CurrentPixel = 0;
        ImageCompressed = true;
        std::uint8_t ChunkHeader = {0};
        int BytesPerPixel = (BitsPerPixel / 8);
        imageData = new char[width * height * sizeof(PixelInfo)];

        do
        {
            ifile->read(reinterpret_cast<char*>(&ChunkHeader), sizeof(ChunkHeader));

            if(ChunkHeader < 128)
            {
                ++ChunkHeader;
                for(int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    ifile->read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                    imageData[CurrentByte++] = Pixel.B;
                    imageData[CurrentByte++] = Pixel.G;
                    imageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24) imageData[CurrentByte++] = Pixel.A;
                }
            }
            else
            {
                ChunkHeader -= 127;
                ifile->read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

                for(int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
                {
                    imageData[CurrentByte++] = Pixel.B;
                    imageData[CurrentByte++] = Pixel.G;
                    imageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24) imageData[CurrentByte++] = Pixel.A;
                }
            }
        } while(CurrentPixel < (width * height));
    }
    else
    {
        Log::Error(LOG_TAG, "Invalid File Format. Required: 24 or 32 Bit TGA File: %s", relative_path);
        return NULL;
    }

    ifile->close();

    //BGR to RGB
    char temp;
    int pos;
    for (int i = 0; i < width*height; i++){
        (!HasAlphaChannel()) ? pos = i * 3 : pos = i * 4;
        temp = imageData[pos  ];
        imageData[pos  ] = imageData[pos+2];
        imageData[pos+2] = temp;
    }

    int type = S_COLOR_RGB;
    if (this->HasAlphaChannel()){
        type = S_COLOR_RGB_ALPHA;
    }

    return new TextureFile((int)width, (int)height, (int)size, type, imageData);
}




