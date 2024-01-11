#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_NO_THREAD_LOCALS // without this broken texture y orientation in Emscripten
#include "stb_image.h"
