#include "ObjectRender.h"

#include "Engine.h"
#include "gles2/GLES2Object.h"

using namespace Supernova;


ObjectRender::ObjectRender(){
    minBufferSize = 0;
    primitiveType = 0;
    buffers.clear();
    vertexAttributes.clear();
    textures.clear();
    indexAttribute.reset();
    properties.clear();
    programShader = -1;
    programDefs = 0;
    lineWidth = 1.0;

    vertexSize = 0;
    
    sceneRender = NULL;

    program = NULL;
    parent = NULL;
}

ObjectRender* ObjectRender::newInstance(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Object();
    }
    
    return NULL;
}

ObjectRender::~ObjectRender(){
    program.reset();
}

void ObjectRender::setProgram(std::shared_ptr<ProgramRender> program){
    this->program = program;
}

void ObjectRender::setParent(ObjectRender* parent){
    this->parent = parent;
    this->primitiveType = parent->primitiveType;
};

void ObjectRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void ObjectRender::setVertexSize(unsigned int vertexSize){
    this->vertexSize = vertexSize;
}

void ObjectRender::setMinBufferSize(unsigned int minBufferSize){
    this->minBufferSize = minBufferSize;
}

void ObjectRender::setPrimitiveType(int primitiveType){
    this->primitiveType = primitiveType;
}

void ObjectRender::setProgramShader(int programShader){
    this->programShader = programShader;
}

void ObjectRender::setLineWidth(float lineWidth){
    this->lineWidth = lineWidth;
}

void ObjectRender::addProgramDef(int programDef){
    this->programDefs |= programDef;
}

void ObjectRender::addBuffer(std::string name, unsigned int size, void* data, int type, bool dynamic){
    if (!name.empty() && data && (size > 0))
        buffers[name] = { size, data, type, dynamic };
}

void ObjectRender::addVertexAttribute(int type, std::string buffer, unsigned int elements, DataType dataType, unsigned int stride, size_t offset){
    if ((!buffer.empty()) && (elements > 0))
        vertexAttributes[type] = { buffer, elements, stride, offset, 0, dataType};
}

void ObjectRender::setIndices(std::string buffer, size_t size, size_t offset, DataType type){
    if (!buffer.empty()) {
        indexAttribute = std::make_shared<AttributeData>(AttributeData{buffer, 1, 0, offset, size, type});
    }
}

void ObjectRender::addProperty(int type, int datatype, unsigned int size, void* data){
    if (data && (size > 0))
        properties[type] = { datatype, size, data };
}

void ObjectRender::addTexture(int type, Texture* texture){
    if (texture) {
        textures[type].clear();
        textures[type].push_back(texture);
    }
}

void ObjectRender::addTextureVector(int type, std::vector<Texture*> texturesVec){
    if (texturesVec.size() > 0) {
        textures[type] = texturesVec;
    }
}

void ObjectRender::updateBuffer(std::string name, unsigned int size, void* data){
    if (buffers.count(name)){
        buffers[name].size = size;
        buffers[name].data = data;
    }
}

std::shared_ptr<ProgramRender> ObjectRender::getProgram(){
    
    loadProgram();
    
    return program;
}

void ObjectRender::checkFog(){
    if (properties.count(S_PROPERTY_FOG_MODE)){
        programDefs |= S_PROGRAM_USE_FOG;
    }
}

void ObjectRender::checkTextureCoords(){
    if (textures.count(S_TEXTURESAMPLER_DIFFUSE))
        if (textures[S_TEXTURESAMPLER_DIFFUSE].size() > 0)
            programDefs |= S_PROGRAM_USE_TEXCOORD;
}

void ObjectRender::checkTextureRect(){
    if (vertexAttributes.count(S_VERTEXATTRIBUTE_TEXTURERECTS)){
        programDefs |= S_PROGRAM_USE_TEXRECT;
    }
    if (properties.count(S_PROPERTY_TEXTURERECT)){
        programDefs |= S_PROGRAM_USE_TEXRECT;
    }
}

void ObjectRender::checkTextureCube(){
    if (textures.count(S_TEXTURESAMPLER_DIFFUSE)) {
        for (size_t i = 0; i < textures[S_TEXTURESAMPLER_DIFFUSE].size(); i++) {
            if (textures[S_TEXTURESAMPLER_DIFFUSE][i]->getType() == S_TEXTURE_CUBE)
                programDefs |= S_PROGRAM_USE_TEXCUBE;
        }
    }
}

void ObjectRender::checkSkinning(){
    if (properties.count(S_PROPERTY_BONESMATRIX)) {
        programDefs |= S_PROGRAM_USE_SKINNING;
    }
}

void ObjectRender::checkMorphTarget(){
    if (vertexAttributes.count(S_VERTEXATTRIBUTE_MORPHTARGET0)) {
        programDefs |= S_PROGRAM_USE_MORPHTARGET;
    }
}

void ObjectRender::checkMorphNormal(){
    if (vertexAttributes.count(S_VERTEXATTRIBUTE_MORPHNORMAL0)) {
        programDefs |= S_PROGRAM_USE_MORPHNORMAL;
    }
}

int ObjectRender::getSizeProperty(int property){
    if (properties.count(property)) {
        return properties[property].size;
    }

    return 0;
}

void ObjectRender::loadProgram(){
    if (!parent) {
        checkFog();
        checkSkinning();
        checkMorphTarget();
        checkMorphNormal();
        checkTextureCoords();
        checkTextureRect();
        checkTextureCube();

        int numPointLights = 0;
        int numSpotLights = 0;
        int numDirLights = 0;
        int numShadows2D = 0;
        int numShadowsCube = 0;
        if (properties.count(S_PROPERTY_AMBIENTLIGHT) && !(programDefs & S_PROGRAM_IS_SKY)) {
            numPointLights = getSizeProperty(S_PROPERTY_POINTLIGHT_POWER);
            numSpotLights = getSizeProperty(S_PROPERTY_SPOTLIGHT_POWER);
            numDirLights = getSizeProperty(S_PROPERTY_DIRLIGHT_POWER);
            numShadows2D = getSizeProperty(S_PROPERTY_SHADOWBIAS2D);
            numShadowsCube = getSizeProperty(S_PROPERTY_SHADOWBIASCUBE);
        }
        int numBlendMapColors = getSizeProperty(S_PROPERTY_BLENDMAPCOLORINDEX);

        std::string shaderStr = std::to_string(programShader);
        shaderStr += "|" + std::to_string(programDefs);
        shaderStr += "|" + std::to_string(numPointLights);
        shaderStr += "|" + std::to_string(numSpotLights);
        shaderStr += "|" + std::to_string(numDirLights);
        shaderStr += "|" + std::to_string(numShadows2D);
        shaderStr += "|" + std::to_string(numShadowsCube);
        shaderStr += "|" + std::to_string(numBlendMapColors);

        program = ProgramRender::sharedInstance(shaderStr);
        if (program && !program.get()->isLoaded()) {
            program.get()->createProgram(programShader, programDefs, numPointLights, numSpotLights, numDirLights, numShadows2D, numShadowsCube, numBlendMapColors);
        }
    }else{
        program = parent->program;
    }
}

bool ObjectRender::isUseTexture(){
    if (textures.count(S_TEXTURESAMPLER_DIFFUSE)){
        return true;
    }
    if (parent){
        if (parent->textures.count(S_TEXTURESAMPLER_DIFFUSE)){
            return true;
        }
    }

    return false;
};

bool ObjectRender::load() {

    loadProgram();
    for ( const auto &p : textures ) {
        for (auto it = textures[p.first].begin(); it != textures[p.first].end(); it++) {
            //Remove if texture cannot be loaded
            if (!(*it)->load())
                textures[p.first].erase(it--);
        }
    }
    
    return true;
}

bool ObjectRender::prepareDraw(){

    return true;
}

bool ObjectRender::draw(){

    return true;
}

bool ObjectRender::finishDraw(){

    return true;
}

void ObjectRender::destroy(){

    for (size_t i = 0; i < textures[S_TEXTURESAMPLER_DIFFUSE].size(); i++) {
        textures[S_TEXTURESAMPLER_DIFFUSE][i]->destroy();
    }

    program.reset();
    ProgramRender::deleteUnused();

    buffers.clear();
    vertexAttributes.clear();
    indexAttribute.reset();
    properties.clear();
    textures.clear();
}
