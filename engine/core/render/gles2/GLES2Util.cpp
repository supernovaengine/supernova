#include "GLES2Util.h"

#include "platform/Log.h"
#include <limits>
#include <algorithm>

GLES2Util::GLES2Util() {
	// TODO Auto-generated constructor stub

}

GLES2Util::~GLES2Util() {
	// TODO Auto-generated destructor stub
}


void GLES2Util::checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        Log::Error(LOG_TAG,"after %s() glError (0x%x)\n", op, error);
    }
}

GLuint GLES2Util::createVBO(GLenum target, const GLsizeiptr size, const GLvoid* data, const GLenum usage) {
    //assert(data != NULL);
	GLuint vbo_object;
	glGenBuffers(1, &vbo_object);
	//assert(vbo_object != 0);

	glBindBuffer(target, vbo_object);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);

	return vbo_object;
}


void GLES2Util::updateVBO(GLuint vbo_object, GLenum target, const GLsizeiptr size, const GLvoid* data) {
    
    glBindBuffer(target, vbo_object);
    glBufferSubData(target, 0, size, data);
    glBindBuffer(target, 0);

}

