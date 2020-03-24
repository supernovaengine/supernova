#include "Mesh.h"
#include "Scene.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Mesh::Mesh(): GraphicObject(){
    dynamic = false;

    defaultBuffer = "vertices";
}

Mesh::~Mesh(){
    destroy();
    removeAllSubmeshes();

    if (render)
        delete render;

    if (shadowRender)
        delete shadowRender;
}

std::vector<Submesh*> Mesh::getSubmeshes(){
    return submeshes;
}

bool Mesh::isDynamic(){
    return dynamic;
}

int Mesh::getPrimitiveType(){
    return primitiveType;
}

void Mesh::setPrimitiveType(int primitiveType){
    this->primitiveType = primitiveType;
}

bool Mesh::resizeSubmeshes(unsigned int count, Material* material){
    bool resized = false;

    for (int i = 0; i < count; i++){
        if (i > (this->submeshes.size()-1)){
            if (material){
                this->submeshes.push_back(new Submesh(material));
            } else {
                this->submeshes.push_back(new Submesh());
            }
            resized = true;
        }
    }

    return resized;
}

void Mesh::updateBuffers(){
    for (auto const& buf : buffers) {
        updateBuffer(buf.first);
    }
}
/*
void Mesh::sortTransparentSubmeshes(){

    if (transparent && scene && scene->isUseDepth() && scene->getUserDefinedTransparency() != S_OPTION_NO){

        bool needSort = false;
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (buffers.count("indices") > 0 && buffers["indices"]->getSize() > 0){
                //TODO: Check if buffer has vertices attributes
                Vector3 submeshFirstVertice = buffers[defaultBuffer]->getVector3(
                        S_VERTEXATTRIBUTE_VERTICES,
                        //TODO: Check when buffer is not unsigned int
                        buffers["indices"]->getUInt(S_INDEXATTRIBUTE, this->submeshes[i]->indices.offset / sizeof(unsigned int)));
                submeshFirstVertice = modelMatrix * submeshFirstVertice;

                if (this->submeshes[i]->getMaterial()->isTransparent()){
                    this->submeshes[i]->distanceToCamera = (this->cameraPosition - submeshFirstVertice).length();
                    needSort = true;
                }
            }
        }

        if (needSort){
            std::sort(submeshes.begin(), submeshes.end(),
                    [](const Submesh* a, const Submesh* b) -> bool
                    {
                        if (a->distanceToCamera == -1)
                            return true;
                        if (b->distanceToCamera == -1)
                            return false;
                        return a->distanceToCamera > b->distanceToCamera;
                    });

        }
    }

}
*/
void Mesh::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    GraphicObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    //sortTransparentSubmeshes();
}

void Mesh::updateModelMatrix(){
    GraphicObject::updateModelMatrix();
    
    this->normalMatrix = modelMatrix.inverse().transpose();

    //sortTransparentSubmeshes();
}

void Mesh::removeAllSubmeshes(){
    for (std::vector<Submesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::textureLoad(){
    if (!GraphicObject::textureLoad())
        return false;
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->textureLoad();
    }
    
    return true;
}

bool Mesh::renderLoad(bool shadow){

    if (!shadow){

        instanciateRender();

        //Activating texture if any submesh has texture
        for (unsigned int i = 0; i < submeshes.size(); i++){
            if (submeshes.at(i)->getMaterial()->getTexture()){
                render->addProgramDef(S_PROGRAM_USE_TEXCOORD);
                if (submeshes.at(i)->getMaterial()->getTexture()->getType() == S_TEXTURE_CUBE){
                    render->addProgramDef(S_PROGRAM_USE_TEXCUBE);
                }
            }
            if (submeshes.at(i)->getMaterial()->getTextureRect()){
                render->addProgramDef(S_PROGRAM_USE_TEXRECT);
            }
        }

        render->setPrimitiveType(primitiveType);
        render->setProgramShader(S_SHADER_MESH);

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->dynamic = dynamic;
            if (submeshes.size() == 1){
                //Use the same render for submesh
                submeshes[i]->setSubmeshRender(render);
            }else{
                submeshes[i]->getSubmeshRender()->setParent(render);
            }
            submeshes[i]->renderLoad(shadow);
        }

    } else {

        instanciateShadowRender();

        shadowRender->setPrimitiveType(primitiveType);
        shadowRender->setProgramShader(S_SHADER_DEPTH_RTT);

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->dynamic = dynamic;
            if (submeshes.size() == 1) {
                //Use the same render for submesh
                submeshes[i]->setSubmeshShadowRender(shadowRender);
            } else {
                submeshes[i]->getSubmeshShadowRender()->setParent(shadowRender);
            }
            submeshes[i]->renderLoad(shadow);
        }

    }

    GraphicObject::renderLoad(shadow);

    //Load submeshes if use a different render
    if (submeshes.size() > 1) {
        if (!shadow) {
            for (size_t i = 0; i < submeshes.size(); i++) {
                submeshes[i]->getSubmeshRender()->load();
            }
        } else {
            for (size_t i = 0; i < submeshes.size(); i++) {
                submeshes[i]->getSubmeshShadowRender()->load();
            }
        }
    }

    return true;
}

bool Mesh::renderDraw(bool shadow){

    if (!visible)
        return false;

    if (!shadow) {

        render->prepareDraw();

        if (submeshes.size() == 1){
            submeshes[0]->getSubmeshRender()->draw();
        }else{
            for (size_t i = 0; i < submeshes.size(); i++) {
                if (submeshes[i]->isVisible()) {
                    submeshes[i]->getSubmeshRender()->prepareDraw();
                    submeshes[i]->getSubmeshRender()->draw();
                    submeshes[i]->getSubmeshRender()->finishDraw();
                }
            }
        }

        render->finishDraw();

    }else{

        shadowRender->prepareDraw();

        if (submeshes.size() == 1){
            submeshes[0]->getSubmeshShadowRender()->draw();
        }else{
            for (size_t i = 0; i < submeshes.size(); i++) {
                if (submeshes[i]->isVisible()) {
                    submeshes[i]->getSubmeshShadowRender()->prepareDraw();
                    submeshes[i]->getSubmeshShadowRender()->draw();
                    submeshes[i]->getSubmeshShadowRender()->finishDraw();
                }
            }
        }

        shadowRender->finishDraw();

    }

    return true;
}

bool Mesh::load(){
    if (scene && scene->isLoadedShadow()) {
        renderLoad(true);
    }

    bool loadReturn = GraphicObject::load();

    //Check after texture is loaded
    for (unsigned int i = 0; i < submeshes.size(); i++){
        if (submeshes.at(i)->getMaterial()->isTransparent()) {
            transparent = true;
        }
    }

    setSceneTransparency(transparent);

    return loadReturn;
}

void Mesh::destroy(){

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }

    GraphicObject::destroy();
}
