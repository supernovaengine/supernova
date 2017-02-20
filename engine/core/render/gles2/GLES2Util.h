#ifndef gles2util_h
#define gles2util_h

#include "GLES2Header.h"
#include <vector>
#include "math/Vector2.h"

#define BUFFER_OFFSET(i) ((void*)(i))

class GLES2Util {
public:
    GLES2Util();
    virtual ~GLES2Util();

    static void checkGlError(const char* op);
    static GLuint createVBO(GLenum target, const GLsizeiptr size, const GLvoid* data, const GLenum usage);
    static void updateVBO(GLuint vbo_object, GLenum target, const GLsizeiptr size, const GLvoid* data);

};

#endif /* gles2util_h */
