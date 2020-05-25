
#ifndef ImageReader_h
#define ImageReader_h

#include "TextureData.h"
#include "file/Data.h"
#include <fstream>

namespace Supernova {

    class ImageReader{

    public:

        virtual TextureData* getRawImage(Data* filedata) = 0;

        virtual ~ImageReader();

    };
    
}


#endif /* ImageRead_h */
