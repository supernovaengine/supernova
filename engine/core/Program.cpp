#include "Program.h"

#include "render/ProgramRender.h"

using namespace Supernova;

Program::Program(){
    this->programRender = NULL;
    
    this->shader = "";
    this->definitions = "";
    
    this->shaderType = 0;
    this->hasLight = false;
    this->hasFog = false;
    this->hasTextureRect = false;
}

Program::Program(std::string shader, std::string definitions): Program(){
    this->shader = shader;
    this->definitions = definitions;
}


void Program::setShader(std::string shader){
    this->shader = shader;
}

void Program::setShader(int shaderType){
    this->shaderType = shaderType;
}

void Program::setDefinitions(std::string definitions){
    this->definitions = definitions;
}

void Program::setDefinitions(bool hasLight, bool hasFog, bool hasTextureRect){
    this->hasLight = hasLight;
    this->hasFog = hasFog;
    this->hasTextureRect = hasTextureRect;
}

std::string Program::getShader(){
    return shader;
}

std::string Program::getDefinitions(){
    return definitions;
}

Program::~Program(){
    programRender.reset();
}

Program::Program(const Program& p){
    programRender = p.programRender;
    shader = p.shader;
    definitions = p.definitions;
    
    shaderType = p.shaderType;
    hasLight = p.hasLight;
    hasFog = p.hasFog;
    hasTextureRect = p.hasTextureRect;
}

Program& Program::operator = (const Program& p){
    programRender = p.programRender;
    shader = p.shader;
    definitions = p.definitions;
    
    shaderType = p.shaderType;
    hasLight = p.hasLight;
    hasFog = p.hasFog;
    hasTextureRect = p.hasTextureRect;
    
    return *this;
}

bool Program::existVertexAttribute(int vertexAttribute){
    if (std::find(shaderVertexAttributes.begin(), shaderVertexAttributes.end(),vertexAttribute)!=shaderVertexAttributes.end()){
        return true;
    }
    return false;
}

bool Program::existProperty(int property){
    if (std::find(shaderProperties.begin(), shaderProperties.end(),property)!=shaderProperties.end()){
        return true;
    }
    return false;
}

std::shared_ptr<ProgramRender> Program::getProgramRender(){
    return programRender;
}

bool Program::load(){
    
    if (this->shader != ""){
    
        programRender = ProgramRender::sharedInstance(shader + "|" + definitions);
        
        if (!programRender.get()->isLoaded()){
            
            programRender.get()->createProgram(shader, definitions);
            
            return true;
        }
    }else{
        std::string shaderStr;
        
        shaderStr = std::to_string(shaderType);
        if (hasLight)
            shaderStr += "|hasLight";
        if (hasFog)
            shaderStr += "|hasFog";
        if (hasTextureRect)
            shaderStr += "|hasTextureRect";
        
        programRender = ProgramRender::sharedInstance(shaderStr);
        
        if (!programRender.get()->isLoaded()){
            
            programRender.get()->createProgram(shaderType, hasLight, hasFog, hasTextureRect);
            
            shaderVertexAttributes.clear();
            shaderProperties.clear();
            
            shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_VERTICES);
            if (hasLight){
               shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_NORMALS);
            }
            if (shaderType == S_SHADER_POINTS){
                shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_POINTSIZES);
                shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_POINTCOLORS);
                if (hasTextureRect){
                    shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_TEXTURERECTS);
                }
            }
            
            shaderProperties.push_back(S_PROPERTY_MVPMATRIX);
            if (hasLight){
                shaderProperties.push_back(S_PROPERTY_MODELMATRIX);
                shaderProperties.push_back(S_PROPERTY_NORMALMATRIX);
                shaderProperties.push_back(S_PROPERTY_CAMERAPOS);
            }
            
            return true;
        }
    }
    
    return false;
}

void Program::destroy(){
    programRender.reset();
    ProgramRender::deleteUnused();
}
