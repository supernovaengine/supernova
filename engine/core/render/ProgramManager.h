
#ifndef ProgramManager_h
#define ProgramManager_h

#include "gles2/GLES2Program.h"
#include "render/ProgramRender.h"
#include <unordered_map>
#include <string>

namespace Supernova {

    class ProgramManager {
        
    private:

        typedef std::unordered_map< std::string, std::shared_ptr<ProgramRender> >::iterator it_type;
        
        static std::unordered_map< std::string, std::shared_ptr<ProgramRender> > programs;
        
        static ProgramRender* getProgramRender();
        
        static ProgramManager::it_type findToRemove();
    public:
        
        static std::shared_ptr<ProgramRender> useProgram(std::string shaderName, std::string definitions);
        static void deleteUnused();
        
        static void clear();
        
    };
    
}


#endif /* ProgramManager_h */
