#include "GLES2Object.h"
#include "GLES2Util.h"
#include "GLES2Program.h"
#include "GLES2Texture.h"
#include "Engine.h"
#include "Log.h"
#include "buffer/Buffer.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_OFFSET(i) ((void*)(i))

using namespace Supernova;


GLES2Object::GLES2Object(): ObjectRender(){
    
}

GLES2Object::~GLES2Object(){

}

void GLES2Object::loadBuffer(std::string name, BufferData buff){

    BufferGlData vb = vertexBuffersGL[name];

    GLenum usageBuffer = GL_STATIC_DRAW;
    if (buff.dynamic)
        usageBuffer = GL_DYNAMIC_DRAW;

    if (vb.size == 0){
        vb.buffer = GLES2Util::createVBO();
    }

    GLenum target = GL_ARRAY_BUFFER;
    if (buff.type == S_BUFFERTYPE_INDEX){
        target = GL_ELEMENT_ARRAY_BUFFER;
    }

    if (vb.size >= buff.size){
        GLES2Util::updateVBO(vb.buffer, target, buff.size, buff.data);
    }else{
        vb.size = std::max((unsigned int)buff.size, minBufferSize);
        GLES2Util::dataVBO(vb.buffer, target, vb.size, buff.data, usageBuffer);
    }

    vertexBuffersGL[name] = vb;
}

GLES2Object::BufferGlData GLES2Object::getVertexBufferGL(std::string name){
    if (parent && ((GLES2Object*)parent)->vertexBuffersGL.count(indexAttribute->bufferName)) {
        return ((GLES2Object*)parent)->vertexBuffersGL[name];
    }else{
        return vertexBuffersGL[name];
    }
}

void GLES2Object::updateBuffer(std::string name, unsigned int size, void* data){
    ObjectRender::updateBuffer(name, size, data);
    if (buffers.count(name))
        loadBuffer(name, buffers[name]);
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

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    GLES2Program* programRender = (GLES2Program*)program.get();
    GLuint glesProgram = programRender->getProgram();
    
    useTexture = glGetUniformLocation(glesProgram, "uUseTexture");

    for (std::unordered_map<std::string, BufferData>::iterator it = buffers.begin(); it != buffers.end(); ++it)
    {
        std::string name = it->first;

        loadBuffer(name, it->second);
        //Log::Debug("Load vertex buffer: %s, size: %lu", name.c_str(), it->second.size);
    }
    
    for (std::unordered_map<int, AttributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
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
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET0){
            attribName = "a_morphTarget0";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET1){
            attribName = "a_morphTarget1";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET2){
            attribName = "a_morphTarget2";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET3){
            attribName = "a_morphTarget3";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET4){
            attribName = "a_morphTarget4";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET5){
            attribName = "a_morphTarget5";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET6){
            attribName = "a_morphTarget6";
        }else if (type == S_VERTEXATTRIBUTE_MORPHTARGET7){
            attribName = "a_morphTarget7";
        }else if (type == S_VERTEXATTRIBUTE_MORPHNORMAL0){
            attribName = "a_morphNormal0";
        }else if (type == S_VERTEXATTRIBUTE_MORPHNORMAL1){
            attribName = "a_morphNormal1";
        }else if (type == S_VERTEXATTRIBUTE_MORPHNORMAL2){
            attribName = "a_morphNormal2";
        }else if (type == S_VERTEXATTRIBUTE_MORPHNORMAL3){
            attribName = "a_morphNormal3";
        }

        attributesGL[type].handle = glGetAttribLocation(glesProgram, attribName.c_str());
        //Log::Debug("Load attribute: %s,handle %i", attribName.c_str(), attributesGL[type].handle);
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

    for (std::unordered_map<int, PropertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
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
        }else if (type == S_PROPERTY_MORPHWEIGHTS){
            propertyName = "u_morphWeights";
        }else if (type == S_PROPERTY_TERRAINTILEOFFSET){
            propertyName = "u_terrainTileOffset";
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
    
    for (std::unordered_map<int, PropertyData>::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        PropertyGlData pb = propertyGL[it->first];
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
    for (std::unordered_map<int, AttributeData>::iterator it = vertexAttributes.begin(); it != vertexAttributes.end(); ++it)
    {
        AttributeGlData att = attributesGL[it->first];
        if (att.handle != -1){
            glEnableVertexAttribArray(att.handle);

            actualBuffer = getVertexBufferGL(it->second.bufferName).buffer;
            if (actualBuffer != lastBuffer)
                glBindBuffer(GL_ARRAY_BUFFER, actualBuffer);
            lastBuffer = actualBuffer;

            GLenum type = 0;
            if (it->second.type == DataType::BYTE){
                type = GL_BYTE;
            }else if (it->second.type == DataType::UNSIGNED_BYTE){
                type = GL_UNSIGNED_BYTE;
            }else if (it->second.type == DataType::SHORT){
                type = GL_SHORT;
            }else if (it->second.type == DataType::UNSIGNED_SHORT){
                type = GL_UNSIGNED_SHORT;
            }else if (it->second.type == DataType::UNSIGNED_INT){
                type = GL_UNSIGNED_INT;
            }else if (it->second.type == DataType::FLOAT){
                type = GL_FLOAT;
            }

            glVertexAttribPointer(att.handle, it->second.elements, type, GL_FALSE, it->second.stride, BUFFER_OFFSET(it->second.offset));
        }
        //Log::Debug("Use attribute handle: %i, elements: %i, stride: %i, offset: %i, from buffer: %s",
        //        att.handle, it->second.elements, it->second.stride, it->second.offset, it->second.bufferName.c_str());
    }
    GLES2Util::checkGlError("Error on bind attribute vertex buffer");

    if (indexAttribute) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getVertexBufferGL(indexAttribute->bufferName).buffer);
    }

    GLES2Util::checkGlError("Error on bind index buffer");

    if (parent){
        textureIndex = ((GLES2Object*)parent)->textureIndex;
    }else{
        textureIndex = 0;
    }


    GLES2Util::checkGlError("Error on bind texture");
    
    return true;
}

bool GLES2Object::draw(){
    if (!ObjectRender::draw()){
        return false;
    }

    //Log::Debug("Start draw");
    
    if (!vertexAttributes.count(S_VERTEXATTRIBUTE_VERTICES) and !indexAttribute){
        Log::Error("Cannot draw object: no vertices");
        return false;
    }
        
    glUniform1i(useTexture, (textures.count(S_TEXTURESAMPLER_DIFFUSE)?true:false));
        
    GLenum modeGles = GL_TRIANGLES;
    if (primitiveType == S_PRIMITIVE_POINTS){
        modeGles = GL_POINTS;
    }
    if (primitiveType == S_PRIMITIVE_LINES){
        modeGles = GL_LINES;
    }
    if (primitiveType == S_PRIMITIVE_TRIANGLE_STRIP){
        modeGles = GL_TRIANGLE_STRIP;
    }

    glLineWidth(lineWidth);

    for ( const auto &p : textures ) {
        std::vector<int> texturesLoc;
        TextureGlData textureData = texturesGL[p.first];

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


    if (indexAttribute) {

        GLenum type = 0;
        if (indexAttribute->type == DataType::UNSIGNED_BYTE){
            type = GL_UNSIGNED_BYTE;
        }else if (indexAttribute->type == DataType::UNSIGNED_SHORT){
            type = GL_UNSIGNED_SHORT;
        }else if (indexAttribute->type == DataType::UNSIGNED_INT){
            type = GL_UNSIGNED_INT;
        }

        glDrawElements(modeGles, (GLsizei) indexAttribute->size, type,
                BUFFER_OFFSET(indexAttribute->offset));
    } else {
        if (vertexSize > 0) {
            glDrawArrays(modeGles, 0, (GLsizei) vertexSize);
        } else {
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
    
    for (std::unordered_map<int, AttributeGlData>::iterator it = attributesGL.begin(); it != attributesGL.end(); ++it)
        if (it->second.handle != -1)
            glDisableVertexAttribArray(it->second.handle);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return true;
}

void GLES2Object::destroy(){

    for (std::unordered_map<std::string, BufferGlData>::iterator it = vertexBuffersGL.begin(); it != vertexBuffersGL.end(); ++it) {
        if (buffers[it->first].data) {
            glDeleteBuffers(1, &it->second.buffer);
            it->second.buffer = -1;
            it->second.size = 0;
        }
    }
    
    for (std::unordered_map<int, AttributeGlData>::iterator it = attributesGL.begin(); it != attributesGL.end(); ++it) {
        if (it->second.handle != -1) {
            it->second.handle = -1;
        }
    }

    vertexBuffersGL.clear();
    attributesGL.clear();
    propertyGL.clear();
    texturesGL.clear();

    ObjectRender::destroy();
}
