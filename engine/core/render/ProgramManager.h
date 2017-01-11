
#ifndef ProgramManager_h
#define ProgramManager_h

#include "gles2/GLES2Program.h"
#include "render/ProgramRender.h"
#include <vector>
#include <string>


class ProgramManager {
    
    typedef struct {
        std::shared_ptr<ProgramRender> value;
        std::string key;
    } ProgramStore;
    
private:
    
    static std::vector<ProgramStore> programs;
    
    static ProgramRender* getProgramRender();
    
    static int findToRemove();
public:
    
    static std::shared_ptr<ProgramRender> useProgram(std::string shaderName, std::string definitions);
    static void deleteUnused();
    
    static void clear();
    
};


#endif /* ProgramManager_h */
