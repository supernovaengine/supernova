#ifndef PROGRAM_H
#define PROGRAM_H

#include "render/ProgramRender.h"

namespace Supernova{

    class Program{
    private:
        std::shared_ptr<ProgramRender> programRender;

    public:
        Program();
        Program(std::shared_ptr<ProgramRender> programRender);
        Program(const Program& p);
        virtual ~Program();

        Program& operator = (const Program& p);

        void setProgramRender(std::shared_ptr<ProgramRender> programRender);

        std::shared_ptr<ProgramRender> getProgramRender();

        void destroy();
    };

}


#endif //PROGRAM_H
