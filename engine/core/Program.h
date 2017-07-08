#ifndef PROGRAM_H
#define PROGRAM_H

#include "render/ProgramRender.h"
#include <string>

namespace Supernova{

    class Program{
    private:
        std::shared_ptr<ProgramRender> programRender;
        
        std::string shader;
        std::string definitions;

    public:
        Program();
        Program(std::string shader, std::string definitions);
        
        void setShader(std::string shader);
        void setDefinitions(std::string definitions);
        
        std::string getShader();
        std::string getDefinitions();
        
        bool load();
        void destroy();
        
        Program(const Program& p);
        virtual ~Program();

        Program& operator = (const Program& p);

        std::shared_ptr<ProgramRender> getProgramRender();

    };

}


#endif //PROGRAM_H
