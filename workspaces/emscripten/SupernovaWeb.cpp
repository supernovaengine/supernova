#include "SupernovaWeb.h"

FILE* SupernovaWeb::platformFopen(const char* fname, const char* mode){
    return fopen(fname, mode);
}