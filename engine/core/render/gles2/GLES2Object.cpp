#include "GLES2Object.h"
#include "GLES2Util.h"
#include "GLES2Program.h"
#include "GLES2Texture.h"
#include "Engine.h"
#include "Log.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_OFFSET(i) ((void*)(i))

using namespace Supernova;


GLES2Object::GLES2Object(): ObjectRender(){
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadVertexBuffer(std::string name, bufferData buff){

    bufferGlData vb = vertexBuffersGL[name];

    GLenum usageBuffer = GL_STATIC_DRAW;
    if (buff.dynamic)
        usageBuffer = GL_DYNAMIC_DRAW;

    if (vb.size == 0){
        vb.buffer = GLES2Util::createVBO();
    }
    if (vb.size >= buff.size){
        GLES2Util::updateVBO(vb.buffer, GL_ARRAY_BUFFER, buff.size, buff.data);
    }else{
        vb.size = std::max((unsigned int)buff.size, minBufferSize);
        GLES2Util::dataVBO(vb.buffer, GL_ARRAY_BUFFER, vb.size, buff.data, usageBuffer);
    }

    vertexBuffersGL[name] = vb;
}

void GLES2Object::loadIndex(indexData ibuff){
    
    indexGlData ib = indexGL;

    GLenum usageBuffer = GL_STATIC_DRAW;
    if (ibuff.dynamic)
        usageBuffer = GL_DYNAMIC_DRAW;
    
    if (ib.size == 0){
        ib.buffer = GLES2Util::createVBO();
    }
    if (ib.size >= ibuff.size){
        GLES2Util::updateVBO(ib.buffer,GL_ELEMENT_ARRAY_BUFFER, ibuff.size * sizeof(unsigned int), ibuff.data);
    }else{
        ib.size = std::max((unsigned int)ibuff.size, minBufferSize);
        GLES2Util::dataVBO(ib.buffer, GL_ELEMENT_ARRAY_BUFFER, ib.size * sizeof(unsigned int), ibuff.data, usageBuffer);
    }

    indexGL = ib;
}

void GLES2Object::updateVertexBuffer(std::string name, unsigned int size, void* data){
    ObjectRender::updateVertexBuffer(name, size, data);
    if (vertexBuffers.count(name))
        loadVertexBuffer(name, vertexBuffers[name]);
}

void GLES2Object::updateIndex(unsigned int size, void* data){
    ObjectRender::updateIndex(size, data);
    if (indexAttribute.data)
        loadIndex(indexAttribute);
}

bool GLES2Object::load(){
    if (!ObjectRender::load()){
        return false;
    }
    //Log::Debug("Start load");

    vertexBuffersGL.clear();
    attributesGL.clear();
    propertyGL.clear();
    texturesGL.clear();
    indexGL.buffer = 0;
    indexGL.size = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    GLES2Program* programRender = (GLES2Program*)program.get();
    GLuint glesProgram = programRender->getProgram();
    
    useTexture = glGetUniformLocation(glesProgram, "uUseTexture");

    for (std::unordered_map<std::string, bufferData>::iterator it = vertexBuffers.begin(); it != vertexBuffers.end(); ++it)
    {
        std::string name = it->first;

        loadVertexBuffer(name, it->second);
        //Log::Debug("Load vertex buffer: %s, size: %lu", name.c_str(), it->second.size);
    }
    
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        int type = it->first;
        
        std::string attribName;
        
        if (type == S_VERTEXATTRIBUTE_VERTICES){
            attribName = "a_Position";
        }else if (type == S_VERTEXATTRIBUTE_TEXTURECOORDS){
            attribName = "a_TextureCoordinates";
        }else if (type == S_VERTEXATTRIBUTE_NORMALS){
            attribName = "a_Normal";
        }else if (type == S_VERTEXATTRIBUTE_BONEWEIGHTS){
            attribName = "a_BoneWeights";
        }else if (type == S_VERTEXATTRIBUTE_BONEIDS){
            attribName = "a_BoneIds";
        }else if (type == S_VERTEXATTRIBUTE_POINTSIZES){
            attribName = "a_pointSize";
        }else if (type == S_VERTEXATTRIBUTE_POINTCOLORS){
            attribName = "a_pointColor";
        }else if (type == S_VERTEXATTRIBUTE_POINTROTATIONS){
            attribName = "a_pointRotation";
        }else if (type == S_VERTEXATTRIBUTE_TEXTURERECTS){
            attribName = "a_textureRect";            
        }

        attributesGL[type].handle = glGetAttribLocation(glesProgram, attribName.c_str());
        //Log::Debug("Load attribute: %s,handle %i", attribName.c_str(), attributesGL[type].handle);
    }
    
    if (indexAttribute.data){
        loadIndex(indexAttribute);
        //Log::Debug("Load index, size: %lu", indexAttribute.size);
    }

    for ( const auto &p : textures ) {
        int type = p.first;

        std::string samplerName;
        unsigned int arraySize = 1;

        if (type == S_TEXTURESAMPLER_DIFFUSE){
            samplerName = "u_TextureUnit";
        }else if (type == S_TEXTURESAMPLER_SHADOWMAP2D){
            samplerName = "u_shadowsMap2D";
            arraySize = programRender->getMaxShadows2D();
        }else if (type == S_TEXTURESAMPLER_SHADOWMAPCUBE){
            samplerName = "u_shadowsMapCube";
            arraySize = programRender->getMaxShadowsCube();
        }

        texturesGL[type].location = glGetUniformLocation(glesProgram, samplerName.c_str());
        texturesGL[type].arraySize = arraySize;
    }

    for (std::unordered_map<int, propertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        int type = it->first;
        
        std::string propertyName;
        
        if (type == S_PROPERTY_MVPMATRIX){
            propertyName = "u_mvpMatrix";
        }else if (type == S_PROPERTY_MODELMATRIX){
            propertyName = "u_mMatrix";
        }else if (type == S_PROPERTY_NORMALMATRIX){
            propertyName = "u_nMatrix";
        }else if (type == S_PROPERTY_DEPTHVPMATRIX){
            propertyName = "u_ShadowVP";
        }else if (type == S_PROPERTY_CAMERAPOS){
            propertyName = "u_EyePos";
        }else if (type == S_PROPERTY_TEXTURERECT){
            propertyName = "u_textureRect";
        }else if (type == S_PROPERTY_COLOR){
            propertyName = "u_Color";
        }else if (type == S_PROPERTY_NUMSHADOWS2D){
            propertyName = "u_NumShadows2D";
        }else if (type == S_PROPERTY_AMBIENTLIGHT){
            propertyName = "u_AmbientLight";
        }else if (type == S_PROPERTY_NUMPOINTLIGHT){
            propertyName = "u_NumPointLight";
        }else if (type == S_PROPERTY_POINTLIGHT_POS){
            propertyName = "u_PointLightPos";
        }else if (type == S_PROPERTY_POINTLIGHT_POWER){
            propertyName = "u_PointLightPower";
        }else if (type == S_PROPERTY_POINTLIGHT_COLOR){
            propertyName = "u_PointLightColor";
        }else if (type == S_PROPERTY_POINTLIGHT_SHADOWIDX){
            propertyName = "u_PointLightShadowIdx";
        }else if (type == S_PROPERTY_NUMSPOTLIGHT){
            propertyName = "u_NumSpotLight";
        }else if (type == S_PROPERTY_SPOTLIGHT_POS){
            propertyName = "u_SpotLightPos";
        }else if (type == S_PROPERTY_SPOTLIGHT_POWER){
            propertyName = "u_SpotLightPower";
        }else if (type == S_PROPERTY_SPOTLIGHT_COLOR){
            propertyName = "u_SpotLightColor";
        }else if (type == S_PROPERTY_SPOTLIGHT_TARGET){
            propertyName = "u_SpotLightTarget";
        }else if (type == S_PROPERTY_SPOTLIGHT_CUTOFF){
            propertyName = "u_SpotLightCutOff";
        }else if (type == S_PROPERTY_SPOTLIGHT_OUTERCUTOFF){
            propertyName = "u_SpotLightOuterCutOff";
        }else if (type == S_PROPERTY_SPOTLIGHT_SHADOWIDX){
            propertyName = "u_SpotLightShadowIdx";
        }else if (type == S_PROPERTY_NUMDIRLIGHT){
            propertyName = "u_NumDirectionalLight";
        }else if (type == S_PROPERTY_DIRLIGHT_DIR){
            propertyName = "u_DirectionalLightDir";
        }else if (type == S_PROPERTY_DIRLIGHT_POWER){
            propertyName = "u_DirectionalLightPower";
        }else if (type == S_PROPERTY_DIRLIGHT_COLOR){
            propertyName = "u_DirectionalLightColor";
        }else if (type == S_PROPERTY_DIRLIGHT_SHADOWIDX){
            propertyName = "u_DirectionalLightShadowIdx";
        }else if (type == S_PROPERTY_FOG_MODE){
            propertyName = "u_fogMode";
        }else if (type == S_PROPERTY_FOG_COLOR){
            propertyName = "u_fogColor";
        }else if (type == S_PROPERTY_FOG_DENSITY){
            propertyName = "u_fogDensity";
        }else if (type == S_PROPERTY_FOG_VISIBILITY){
            propertyName = "u_fogVisibility";
        }else if (type == S_PROPERTY_FOG_START){
            propertyName = "u_fogStart";
        }else if (type == S_PROPERTY_FOG_END){
            propertyName = "u_fogEnd";
        }else if (type == S_PROPERTY_SHADOWLIGHT_POS){
            propertyName = "u_shadowLightPos";
        }else if (type == S_PROPERTY_SHADOWCAMERA_NEARFAR){
            propertyName = "u_shadowCameraNearFar";
        }else if (type == S_PROPERTY_ISPOINTSHADOW){
            propertyName = "u_isPointShadow";
        }else if (type == S_PROPERTY_SHADOWBIAS2D){
            propertyName = "u_shadowBias2D";
        }else if (type == S_PROPERTY_SHADOWBIASCUBE){
            propertyName = "u_shadowBiasCube";
        }else if (type == S_PROPERTY_SHADOWCAMERA_NEARFAR2D){
            propertyName = "u_shadowCameraNearFar2D";
        }else if (type == S_PROPERTY_SHADOWCAMERA_NEARFARCUBE){
            propertyName = "u_shadowCameraNearFarCube";
        }else if (type == S_PROPERTY_NUMCASCADES2D){
            propertyName = "u_shadowNumCascades2D";
        }else if (type == S_PROPERTY_BONESMATRIX){
            propertyName = "u_bonesMatrix";
        }
        
        propertyGL[type].handle = glGetUniformLocation(glesProgram, propertyName.c_str());
        //Log::Debug("Get property handle: %s, size: %lu, handle %i", propertyName.c_str(), it->second.size, propertyGL[type].handle);
    }
    
    GLES2Util::checkGlError("Error on load GLES2");

    return true;
}

bool GLES2Object::prepareDraw(){

    GLES2Program* programRender = (GLES2Program*)program.get();
    GLuint glesProgram = programRender->getProgram();
    if (parent==NULL){
        glUseProgram(glesProgram);
        GLES2Util::checkGlError("glUseProgram");
    }
    
    if (!ObjectRender::prepareDraw()){
        return false;
    }
    //Log::Debug("Start prepare");
    
    for (std::unordered_map<int, propertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        propertyGlData pb = propertyGL[it->first];
        if (pb.handle != -1){
            if (it->second.datatype == S_PROPERTYDATA_FLOAT1){
                glUniform1fv(pb.handle, (GLsizei)it->second.size, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_FLOAT2){
                glUniform2fv(pb.handle, (GLsizei)it->second.size, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_FLOAT3){
                glUniform3fv(pb.handle, (GLsizei)it->second.size, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_FLOAT4){
                glUniform4fv(pb.handle, (GLsizei)it->second.size, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_INT1){
                glUniform1iv(pb.handle, (GLsizei)it->second.size, (GLint*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_INT2){
                glUniform2iv(pb.handle, (GLsizei)it->second.size, (GLint*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_INT3){
                glUniform3iv(pb.handle, (GLsizei)it->second.size, (GLint*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_INT4){
                glUniform4iv(pb.handle, (GLsizei)it->second.size, (GLint*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_MATRIX2){
                glUniformMatrix2fv(pb.handle, (GLsizei)it->second.size, GL_FALSE, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_MATRIX3){
                glUniformMatrix3fv(pb.handle, (GLsizei)it->second.size, GL_FALSE, (GLfloat*)it->second.data);
            }else if (it->second.datatype == S_PROPERTYDATA_MATRIX4){
                glUniformMatrix4fv(pb.handle, (GLsizei)it->second.size, GL_FALSE, (GLfloat*)it->second.data);
            }
        }
        //Log::Debug("Use property handle: %i", propertyGL[it->first].handle);
    }
    GLES2Util::checkGlError("Error on use property on draw");

    GLuint lastBuffer = 0;
    GLuint actualBuffer = 0;
    for (std::unordered_map<int, attributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        attributeGlData att = attributesGL[it->first];
        if (att.handle != -1){
            glEnableVertexAttribArray(att.handle);

            actualBuffer = vertexBuffersGL[it->second.bufferName].buffer;
            if (actualBuffer != lastBuffer)
                glBindBuffer(GL_ARRAY_BUFFER, actualBuffer);
            lastBuffer = actualBuffer;

            glVertexAttribPointer(att.handle, it->second.elements, GL_FLOAT, GL_FALSE, it->second.stride, BUFFER_OFFSET(it->second.offset));
        }
        //Log::Debug("Use attribute handle: %i", att.handle);
    }
    GLES2Util::checkGlError("Error on bind attribute vertex buffer");
    
    if (indexAttribute.data){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexGL.buffer);
    }
    GLES2Util::checkGlError("Error on bind index buffer");

    if (parent){
        textureIndex = ((GLES2Object*)parent)->textureIndex;
    }else{
        textureIndex = 0;
    }

    for ( const auto &p : textures ) {
        std::vector<int> texturesLoc;
        textureGlData textureData = texturesGL[p.first];

        if (textureData.location >= 0) {
            for (size_t i = 0; i < p.second.size(); i++) {
                if (textureIndex < maxTextureUnits) {
                    texturesLoc.push_back(textureIndex);

                    glActiveTexture(GL_TEXTURE0 + textureIndex);
                    glBindTexture(
                            ((GLES2Texture *) (p.second[i]->getTextureRender().get()))->getTextureType(),
                            ((GLES2Texture *) (p.second[i]->getTextureRender().get()))->getTexture());

                    textureIndex++;
                } else {
                    Log::Error("Exceeding max texture units: %i", maxTextureUnits);
                }
            }

            if (texturesLoc.size() > 0) {
                if (texturesLoc.size() <= textureData.arraySize) {

                    while (texturesLoc.size() < textureData.arraySize) {
                        texturesLoc.push_back(texturesLoc[0]);
                    }

                    glUniform1iv(textureData.location, (GLsizei) texturesLoc.size(), &texturesLoc.front());

                } else {
                    Log::Error("Exceeding texture limit of location: %i", textureData.location);
                }
            }
        }
    }

    GLES2Util::checkGlError("Error on bind texture");
    
    return true;
}

bool GLES2Object::draw(){
    if (!ObjectRender::draw()){
        return false;
    }

    //Log::Debug("Start draw");
    
    if ((!vertexAttributes.count(S_VERTEXATTRIBUTE_VERTICES)) and (indexAttribute.size == 0)){
        Log::Error("Cannot draw object: no vertices or indices");
        return false;
    }
        
    glUniform1i(useTexture, (textures.count(S_TEXTURESAMPLER_DIFFUSE)?true:false));
        
    GLenum modeGles = GL_TRIANGLES;
    if (primitiveType == S_PRIMITIVE_TRIANGLES_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }
    if (primitiveType == S_PRIMITIVE_POINTS){
        modeGles = GL_POINTS;
    }
    
    if (indexAttribute.data){
        glDrawElements(modeGles, (GLsizei)indexAttribute.size, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    }else{
        if (vertexSize > 0) {
            glDrawArrays(modeGles, 0, (GLsizei) vertexSize);
        }else{
            Log::Error("Cannot draw object, vertex size is 0");
        }
    }
    
    GLES2Util::checkGlError("Error on draw GLES2");

    return true;
}

bool GLES2Object::finishDraw(){
    if (!ObjectRender::finishDraw()){
        return false;
    }
    
    for (std::unordered_map<int, attributeGlData>::iterator it = attributesGL.begin(); it != attributesGL.end(); ++it)
        if (it->second.handle != -1)
            glDisableVertexAttribArray(it->second.handle);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return true;
}

void GLES2Object::destroy(){

    for (std::unordered_map<std::string, bufferGlData>::iterator it = vertexBuffersGL.begin(); it != vertexBuffersGL.end(); ++it) {
        if (vertexBuffers[it->first].data) {
            glDeleteBuffers(1, &it->second.buffer);
            it->second.buffer = -1;
            it->second.size = 0;
        }
    }
    
    for (std::unordered_map<int, attributeGlData>::iterator it = attributesGL.begin(); it != attributesGL.end(); ++it) {
        if (it->second.handle != -1) {
            it->second.handle = -1;
        }
    }
    
    if (indexAttribute.data){
        glDeleteBuffers(1, &indexGL.buffer);
        indexGL.buffer = -1;
        indexGL.size = 0;
    }

    ObjectRender::destroy();
}
