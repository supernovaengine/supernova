#include "GLES2Util.h"

#include "platform/Log.h"
#include <limits>
#include <algorithm>

using namespace Supernova;

GLuint GLES2Util::emptyTexture;
bool GLES2Util::emptyTextureLoaded;

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

void GLES2Util::generateEmptyTexture(){
    if (!GLES2Util::emptyTextureLoaded){
        glGenTextures(1, &GLES2Util::emptyTexture);
        glBindTexture(GL_TEXTURE_2D, GLES2Util::emptyTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1, 1, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GLES2Util::emptyTextureLoaded = true;
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

