#include "Log.h"
#define SOKOL_LOG(s) { SOKOL_ASSERT(s); Supernova::Log::error(s); }

#define SOKOL_IMPL

#if defined(SUPERNOVA_ANDROID)
// instancing is not working on Android and GLES2
// can not use internal Sokol GL Header because gl2ext.h
#define SOKOL_EXTERNAL_GL_LOADER
#include <GLES2/gl2.h>
#endif

#if defined(_WIN32)
#define SOKOL_LOG(s) OutputDebugStringA(s)
#endif
/* this is only needed for the debug-inspection headers */
#define SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */

#ifdef SUPERNOVA_SOKOL
    #include "sokol_app.h"
#endif

#include "sokol_gfx.h"
#include "sokol_time.h"
//#include "sokol_audio.h"

#ifdef SUPERNOVA_SOKOL
    #include "sokol_fetch.h"
    #include "sokol_glue.h"
#endif
