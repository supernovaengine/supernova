

#ifndef JPEGRead_h
#define JPEGRead_h

#include "ImageRead.h"
#include <stdio.h>

class JPEGRead: public ImageRead{
    
public:
    
    RawImage getRawImage(FILE* file);
    
};

#endif /* JPEGRead_h */
