
#ifndef ProgramManager_h
#define ProgramManager_h

#include "gles2/GLES2Program.h"
#include "render/ProgramRender.h"
#include "Program.h"
#include <unordered_map>
#include <string>

namespace Supernova {

    class ProgramManager {
        
    private:

        typedef std::unordered_map<std::string, Program>::iterator it_type;
        static std::unordered_map<std::string, Program> programs;
        
        static ProgramRender* getProgramRender();
        static ProgramManager::it_type findToRemove();

    public:
        
        static Program useProgram(std::string shaderName, std::string definitions);
        static void deleteUnused();
        
        static void clear();
        
    };
    
}


#endif /* ProgramManager_h */
