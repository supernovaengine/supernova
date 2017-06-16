
#ifndef ImageReader_h
#define ImageReader_h

#include "TextureFile.h"
#include "FileData.h"
#include <fstream>

namespace Supernova {

    class ImageReader{

    public:

        virtual TextureFile* getRawImage(FileData* filedata) = 0;

        virtual ~ImageReader();

    };
    
}


#endif /* ImageRead_h */
