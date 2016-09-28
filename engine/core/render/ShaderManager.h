
#ifndef ShaderManager_h
#define ShaderManager_h

#include "gles2/GLES2Program.h"
#include "render/ShaderRender.h"
#include <vector>
#include <string>


class ShaderManager {
    
    typedef struct {
        ShaderRender* value;
        std::string key;
        int reference;
    } ShaderStore;
    
private:
    
    static std::vector<ShaderStore> shaders;
    
    ShaderRender* getShaderRender();
public:
    
    ShaderRender* useShader(std::string shaderName);
    void deleteShader(std::string shaderName);
    
    static void clear();
    
};


#endif /* ShaderManager_h */
