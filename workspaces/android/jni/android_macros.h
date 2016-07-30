/*
 * android_macros.h
 *
 *  Created on: 17 de out de 2015
 *      Author: eduardolima
 */

#ifndef JNI_ANDROID_MACROS_H_
#define JNI_ANDROID_MACROS_H_

#ifndef SUPERNOVA_ANDROID
#define SUPERNOVA_ANDROID
#endif

#define LOG_TAG "Supernova"

#define UNUSED(x) (void)(x)

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef __ANDROID__
#include <stdio.h>
#include <android/log.h>
#include "android_fopen.h"

#define fopen(name, mode) android_fopen(name, mode)

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define fprintf(stdout, ...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define perror(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define sdl_logerr(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define dolog(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define error_report(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define error_printf(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define error_vprintf(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#endif


#ifdef  __cplusplus
}
#endif


#endif /* JNI_ANDROID_MACROS_H_ */
