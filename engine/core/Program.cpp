#include "Program.h"

#include "render/ProgramRender.h"

using namespace Supernova;

Program::Program(){
    this->programRender = NULL;
    
    this->shaderType = 0;
    this->numLights = 0;
    this->hasFog = false;
    this->hasTextureCoords = false;
    this->hasTextureRect = false;
    this->hasTextureCube = false;
    this->isSky = false;
    this->isText = false;
    this->numShadows2D = 0;
    this->numShadowsCube = 0;
}

void Program::setShader(int shaderType){
    this->shaderType = shaderType;
}

void Program::setDefinitions(int numLights, int numShadows2D, int numShadowsCube, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText){
    this->numLights = numLights;
    this->hasFog = hasFog;
    this->hasTextureCoords = hasTextureCoords;
    this->hasTextureRect = hasTextureRect;
    this->hasTextureCube = hasTextureCube;
    this->isSky = isSky;
    this->isText = isText;
    this->numShadows2D = numShadows2D;
    this->numShadowsCube = numShadowsCube;
}

Program::~Program(){
    programRender.reset();
}

Program::Program(const Program& p){
    programRender = p.programRender;
    
    shaderType = p.shaderType;
    numLights = p.numLights;
    hasFog = p.hasFog;
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
    numShadows2D = p.numShadows2D;
    numShadowsCube = p.numShadowsCube;
}

Program& Program::operator = (const Program& p){
    programRender = p.programRender;
    
    shaderType = p.shaderType;
    numLights = p.numLights;
    hasFog = p.hasFog;
    hasTextureCoords = p.hasTextureCoords;
    hasTextureRect = p.hasTextureRect;
    hasTextureCube = p.hasTextureCube;
    isSky = p.isSky;
    isText = p.isText;
    numShadows2D = p.numShadows2D;
    numShadowsCube = p.numShadowsCube;
    
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
    if (numLights > 0)
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
    if (numShadows2D > 0)
        shaderStr += "|hasShadows2D";
    if (numShadowsCube > 0)
        shaderStr += "|hasShadowsCube";

    programRender = ProgramRender::sharedInstance(shaderStr);

    if (!programRender.get()->isLoaded()){

        programRender.get()->createProgram(shaderType, numLights, numShadows2D, numShadowsCube, hasFog, hasTextureCoords, hasTextureRect, hasTextureCube, isSky, isText);

    }

    shaderVertexAttributes.clear();
    shaderProperties.clear();

    // Attributes of program
    if (shaderType == S_SHADER_POINTS || shaderType == S_SHADER_MESH) {

        shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_VERTICES);
        if (numLights > 0) {
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
        if (numLights > 0) {
            shaderProperties.push_back(S_PROPERTY_MODELMATRIX);
            shaderProperties.push_back(S_PROPERTY_NORMALMATRIX);
            shaderProperties.push_back(S_PROPERTY_CAMERAPOS);

            shaderProperties.push_back(S_PROPERTY_AMBIENTLIGHT);

            shaderProperties.push_back(S_PROPERTY_NUMPOINTLIGHT);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_POS);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_COLOR);
            shaderProperties.push_back(S_PROPERTY_POINTLIGHT_SHADOWIDX);

            shaderProperties.push_back(S_PROPERTY_NUMSPOTLIGHT);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_POS);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_COLOR);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_TARGET);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_CUTOFF);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_OUTERCUTOFF);
            shaderProperties.push_back(S_PROPERTY_SPOTLIGHT_SHADOWIDX);

            shaderProperties.push_back(S_PROPERTY_NUMDIRLIGHT);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_DIR);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_POWER);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_COLOR);
            shaderProperties.push_back(S_PROPERTY_DIRLIGHT_SHADOWIDX);
        }

        if (hasFog) {
            shaderProperties.push_back(S_PROPERTY_FOG_MODE);
            shaderProperties.push_back(S_PROPERTY_FOG_COLOR);
            shaderProperties.push_back(S_PROPERTY_FOG_VISIBILITY);
            shaderProperties.push_back(S_PROPERTY_FOG_DENSITY);
            shaderProperties.push_back(S_PROPERTY_FOG_START);
            shaderProperties.push_back(S_PROPERTY_FOG_END);
        }

        if (numShadows2D > 0) {
            shaderProperties.push_back(S_PROPERTY_DEPTHVPMATRIX);
            shaderProperties.push_back(S_PROPERTY_NUMSHADOWS2D);
            shaderProperties.push_back(S_PROPERTY_SHADOWBIAS2D);
            shaderProperties.push_back(S_PROPERTY_SHADOWCAMERA_NEARFAR2D);
            shaderProperties.push_back(S_PROPERTY_NUMCASCADES2D);
        }

        if (numShadowsCube > 0) {
            shaderProperties.push_back(S_PROPERTY_SHADOWBIASCUBE);
            shaderProperties.push_back(S_PROPERTY_SHADOWCAMERA_NEARFARCUBE);
        }

    }else if (shaderType == S_SHADER_DEPTH_RTT){

        shaderVertexAttributes.push_back(S_VERTEXATTRIBUTE_VERTICES);

        shaderProperties.push_back(S_PROPERTY_MVPMATRIX);
        shaderProperties.push_back(S_PROPERTY_MODELMATRIX);

        shaderProperties.push_back(S_PROPERTY_SHADOWLIGHT_POS);
        shaderProperties.push_back(S_PROPERTY_SHADOWCAMERA_NEARFAR);

        shaderProperties.push_back(S_PROPERTY_ISPOINTSHADOW);

    }
    return true;
}

void Program::destroy(){
    programRender.reset();
    ProgramRender::deleteUnused();
}
