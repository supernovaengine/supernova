#ifndef gles2util_h
#define gles2util_h

#include "GLES2Header.h"
#include <vector>
#include "math/Vector2.h"

namespace Supernova {

    class GLES2Util {
    public:
        
        GLES2Util();
        virtual ~GLES2Util();
        
        static GLuint emptyTexture; //For web bug only
        static bool emptyTextureLoaded;
        
        static void generateEmptyTexture();

        static void checkGlError(const char* op);
        static GLuint createVBO(GLenum target, const GLsizeiptr size, const GLvoid* data, const GLenum usage);
        static GLuint create2VBO();
        static void dataVBO(GLuint vbo_object, GLenum target, const GLsizeiptr size, const GLvoid* data, const GLenum usage);
        static void updateVBO(GLuint vbo_object, GLenum target, const GLsizeiptr size, const GLvoid* data);

    };
    
}

#endif /* gles2util_h */
