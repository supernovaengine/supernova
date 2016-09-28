#ifndef ShaderRender_h
#define ShaderRender_h

#include <string>

class ShaderRender {
    
public:
    
    inline virtual ~ShaderRender(){}
    
    virtual void createShader(std::string shaderName) = 0;
    virtual void deleteShader() = 0;
    
};

#endif /* ShaderRender_h */
