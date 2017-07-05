#include "Program.h"

#include "render/ProgramManager.h"

using namespace Supernova;

Program::Program(){

}

Program::Program(std::shared_ptr<ProgramRender> programRender){
    this->programRender = programRender;
}

Program::~Program(){
    programRender.reset();
}

Program::Program(const Program& p){
    programRender = p.programRender;
}

Program& Program::operator = (const Program& p){
    programRender = p.programRender;

    return *this;
}

void Program::setProgramRender(std::shared_ptr<ProgramRender> programRender){
    this->programRender = programRender;
}

std::shared_ptr<ProgramRender> Program::getProgramRender(){
    return programRender;
}

void Program::destroy(){
    programRender.reset();
    ProgramManager::deleteUnused();
}