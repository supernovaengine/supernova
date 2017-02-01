
#include "BMPReader.h"
#include "ColorType.h"
#include "platform/Log.h"
#include <vector>


TextureFile* BMPReader::getRawImage(const char* relative_path, std::ifstream* ifile){
    
    
    ifile->seekg(0, std::ios::end);
    std::size_t Length = ifile->tellg();
    ifile->seekg(0, std::ios::beg);
    
    //std::vector<std::uint8_t> FileInfo(Length);
    std::vector<std::uint8_t> fileInfo(Length);
    ifile->read(reinterpret_cast<char*>(fileInfo.data()), 54);
    //ifile->read(fileInfo, 54);

    if(fileInfo[0] != 'B' && fileInfo[1] != 'M')
    {
        Log::Error(LOG_TAG, "Invalid File Format. Bitmap Required: %s", relative_path);
        return NULL;
    }

    if (fileInfo[28] != 24 && fileInfo[28] != 32)
    {
        Log::Error(LOG_TAG, "Invalid File Format. 24 or 32 bit Image Required: %s", relative_path);
        return NULL;
    }

    BitsPerPixel = fileInfo[28];
    width = fileInfo[18] + (fileInfo[19] << 8);
    height = fileInfo[22] + (fileInfo[23] << 8);
    std::uint32_t PixelsOffset = fileInfo[10] + (fileInfo[11] << 8);
    std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
    
    char* fileData = new char[size];
    ifile->seekg(PixelsOffset, std::ios::beg);
    ifile->read(fileData, size);
    
    //converting BGR to RGB
    char temp;
    int pos;
    for (int i = 0; i < width*height; i++){
        pos = i * (BitsPerPixel / 8);
        temp = fileData[pos  ];
        fileData[pos  ] = fileData[pos+2];
        fileData[pos+2] = temp;
    }
    
    int type = S_COLOR_RGB;
    if (BitsPerPixel == 32){
        type = S_COLOR_RGB_ALPHA;
    }
    
    return new TextureFile((int)width, (int)height, (int)size, type, BitsPerPixel, fileData);
}
