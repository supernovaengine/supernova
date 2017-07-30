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
    this->hasTextureCoords = false;
    this->hasTextureRect = false;
    this->hasTextureCube = false;
    this->isSky = false;
    this->isText = false;
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

void Program::setDefinitions(bool hasLight, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText){
    this->hasLight = hasLight;
    this->hasFog = hasFog;
    this->hasTextureCoords = hasTextureCoords;
    this->hasTextureRect = hasTextureRect;
    this->hasTextureCube = hasTextureCube;
    this->isSky = isSky;
    this->isText = isText;
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
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
}

Program& Program::operator = (const Program& p){
    programRender = p.programRender;
    shader = p.shader;
    definitions = p.definitions;
    
    shaderType = p.shaderType;
    hasLight = p.hasLight;
    hasFog = p.hasFog;
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
    
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
        if (hasTextureCoords)
            shaderStr += "|hasTextureCoords";
        if (hasTextureRect)
            shaderStr += "|hasTextureRect";
        if (isSky)
            shaderStr += "|isSky";
        if (isText)
            shaderStr += "|isText";
        
        programRender = ProgramRender::sharedInstance(shaderStr);
        
        if (!programRender.get()->isLoaded()){
            
            programRender.get()->createProgram(shaderType, hasLight, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText);
            
            shaderVertexAttributes.clear();
            shaderProperties.clear();
            
            // Attributes of program
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
            if (shaderType == S_SHADER_MESH){
                if (hasTextureCoords){
                    shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_TEXTURECOORDS);
                }
            }
            
            // Properties of program
            if (shaderType == S_SHADER_MESH){
                shaderProperties.push_back(S_PROPERTY_COLOR);
                if (hasTextureRect){
                    shaderProperties.push_back(S_PROPERTY_TEXTURERECT);
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
