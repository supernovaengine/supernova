#ifndef ProgramRender_h
#define ProgramRender_h

#include <string>

class ProgramRender {
    
public:
    
    inline virtual ~ProgramRender(){}
    
    virtual void createProgram(std::string shaderName, std::string definitions) = 0;
    virtual void deleteProgram() = 0;
    
};

#endif /* ProgramRender_h */
