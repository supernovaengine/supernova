#include "Log.h"
#define SOKOL_LOG(s) { SOKOL_ASSERT(s); Supernova::Log::Error(s); }

#define SOKOL_IMPL

#ifdef SUPERNOVA_WEB
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    //TODO: Use WEBGL2 too
    //#include <GLES3/gl3.h>
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
