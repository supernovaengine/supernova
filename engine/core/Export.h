#ifndef SUPERNOVA_EXPORT_H
#define SUPERNOVA_EXPORT_H

#if defined(_MSC_VER)
    #ifdef SUPERNOVA_SHARED
        #ifdef SUPERNOVA_EXPORTS
            #define SUPERNOVA_API __declspec(dllexport)
        #else
            #define SUPERNOVA_API __declspec(dllimport)
        #endif
    #else
        #define SUPERNOVA_API
    #endif
#else
    #define SUPERNOVA_API
#endif

#endif // SUPERNOVA_EXPORT_H