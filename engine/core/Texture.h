#ifndef texture_h
#define texture_h

#include <vector>
#include <limits>
#include <string>
#include "math/Vector2.h"
#include "math/Vector3.h"

class Texture{
private:
    std::string path;
    bool used;
public:
    Texture();
    Texture(const char* path);
    Texture(const Texture& v);
    Texture& operator = (const Texture& v);

    virtual ~Texture();

    void loadFromFile(const char* path);
    const char* getFilePath();

    bool isUsed();
};



#endif /* texture_h */
