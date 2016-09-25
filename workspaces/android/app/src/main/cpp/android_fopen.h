#ifndef ANDROID_FOPEN_H
#define ANDROID_FOPEN_H

#include <android/asset_manager.h>


#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif


void android_fopen_set_asset_manager(AAssetManager* manager);
AAssetManager* android_fopen_get_asset_manager();

FILE* android_fopen(const char* fname, const char* mode);


#ifdef __cplusplus
}
#endif

#endif
