#include "Program.h"

#include "render/ProgramRender.h"

using namespace Supernova;

Program::Program(){
    this->programRender = NULL;
    
    this->shaderType = 0;
    this->hasLight = false;
    this->hasFog = false;
    this->hasTextureCoords = false;
    this->hasTextureRect = false;
    this->hasTextureCube = false;
    this->isSky = false;
    this->isText = false;
    this->hasShadows = false;
}

void Program::setShader(int shaderType){
    this->shaderType = shaderType;
}

void Program::setDefinitions(bool hasLight, bool hasFog, bool hasTextureCoords, bool hasTextureRect,
                             bool hasTextureCube, bool isSky, bool isText, bool hasShadows){
    this->hasLight = hasLight;
    this->hasFog = hasFog;
    this->hasTextureCoords = hasTextureCoords;
    this->hasTextureRect = hasTextureRect;
    this->hasTextureCube = hasTextureCube;
    this->isSky = isSky;
    this->isText = isText;
    this->hasShadows = hasShadows;
}

Program::~Program(){
    programRender.reset();
}

Program::Program(const Program& p){
    programRender = p.programRender;
    
    shaderType = p.shaderType;
    hasLight = p.hasLight;
    hasFog = p.hasFog;
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
    hasShadows = p.hasShadows;
}

Program& Program::operator = (const Program& p){
    programRender = p.programRender;
    
    shaderType = p.shaderType;
    hasLight = p.hasLight;
    hasFog = p.hasFog;
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
    hasShadows = p.hasShadows;
    
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
    if (hasShadows)
        shaderStr += "|hasShadows";

    programRender = ProgramRender::sharedInstance(shaderStr);

    if (!programRender.get()->isLoaded()){

        programRender.get()->createProgram(shaderType, hasLight, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText, hasShadows);

    }

    shaderVertexAttributes.clear();
    shaderProperties.clear();

    // Attributes of program
    if (shaderType == S_SHADER_POINTS || shaderType == S_SHADER_MESH) {

        shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_VERTICES);
        if (hasLight) {
            shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_NORMALS);
        }
        if (shaderType == S_SHADER_POINTS) {
            shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_POINTSIZES);
            shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_POINTCOLORS);
            if (hasTextureRect) {
                shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_TEXTURERECTS);
            }
        }
        if (shaderType == S_SHADER_MESH) {
            if (hasTextureCoords) {
                shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_TEXTURECOORDS);
            }
        }

        // Properties of program
        if (shaderType == S_SHADER_MESH) {
            shaderProperties.push_back(S_PROPERTY_COLOR);
            if (hasTextureRect) {
                shaderProperties.push_back(S_PROPERTY_TEXTURERECT);
            }
        }
        shaderProperties.push_back(S_PROPERTY_MVPMATRIX);
        if (hasLight) {
            shaderProperties.push_back(S_PROPERTY_MODELMATRIX);
            shaderProperties.push_back(S_PROPERTY_NORMALMATRIX);
            shaderProperties.push_back(S_PROPERTY_CAMERAPOS);

            shaderProperties.push_back(S_PROPERTY_AMBIENTLIGHT);

            shaderProperties.push_back(S_PROPERTY_NUMPOINTLIGHT);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_POS);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_COLOR);

            shaderProperties.push_back(S_PROPERTY_NUMSPOTLIGHT);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_POS);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_COLOR);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_TARGET);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_CUTOFF);

            shaderProperties.push_back(S_PROPERTY_NUMDIRLIGHT);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_DIR);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_COLOR);
        }

        if (hasFog) {
            shaderProperties.push_back(S_PROPERTY_FOG_MODE);
            shaderProperties.push_back(S_PROPERTY_FOG_COLOR);
            shaderProperties.push_back(S_PROPERTY_FOG_VISIBILITY);
            shaderProperties.push_back(S_PROPERTY_FOG_DENSITY);
            shaderProperties.push_back(S_PROPERTY_FOG_START);
            shaderProperties.push_back(S_PROPERTY_FOG_END);
        }

        if (hasShadows) {
            shaderProperties.push_back(S_PROPERTY_DEPTHVPMATRIX);
            shaderProperties.push_back(S_PROPERTY_NUMSHADOWS);
        }

    }else if (shaderType == S_SHADER_DEPTH_RTT){

        shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_VERTICES);

        shaderProperties.push_back(S_PROPERTY_MVPMATRIX);

    }
    return true;
}

void Program::destroy(){
    programRender.reset();
    ProgramRender::deleteUnused();
}
