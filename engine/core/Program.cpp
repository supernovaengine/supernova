#include "Program.h"

#include "render/ProgramRender.h"

using namespace Supernova;

Program::Program(){
    this->programRender = NULL;
    
    this->shader = "";
    this->definitions = "";
}

Program::Program(std::string shader, std::string definitions): Program(){
    this->shader = shader;
    this->definitions = definitions;
}


void Program::setShader(std::string shader){
    this->shader = shader;
}

void Program::setDefinitions(std::string definitions){
    this->definitions = definitions;
}

std::string Program::getShader(){
    return shader;
}

std::string Program::getDefinitions(){
    return definitions;
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


std::shared_ptr<ProgramRender> Program::getProgramRender(){
    return programRender;
}

bool Program::load(){
    programRender = ProgramRender::sharedInstance(shader + "|" + definitions);
    
    if (!programRender.get()->isLoaded()){
        programRender.get()->createProgram(shader, definitions);
    }
    
    return true;
}

void Program::destroy(){
    programRender.reset();
    ProgramRender::deleteUnused();
}
