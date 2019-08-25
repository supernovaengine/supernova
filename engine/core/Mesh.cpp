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

void Mesh::setColor(Vector4 color){
    setColor(color, 0);
}

void Mesh::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha), 0);
}

Vector4 Mesh::getColor(){
    return getColor(0);
}

void Mesh::setColor(Vector4 color, int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        if (color.w != 1) {
            transparent = true;
        }
        submeshes[submesh]->getMaterial()->setColor(color);
    }
}

void Mesh::setColor(float red, float green, float blue, float alpha, int submesh){
    setColor(Vector4(red, green, blue, alpha), submesh);
}

Vector4 Mesh::getColor(int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        return *submeshes[submesh]->getMaterial()->getColor();
    }

    return Vector4(0, 0, 0, 0);
}

void Mesh::setTexture(Texture* texture, int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        Texture *oldTexture = submeshes[submesh]->getMaterial()->getTexture();

        if (texture != oldTexture) {
            submeshes[submesh]->getMaterial()->setTexture(texture);
            if (loaded) {
                textureLoad();
            }
        }
    }
}

void Mesh::setTexture(std::string texturepath, int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        std::string oldTexture = submeshes[submesh]->getMaterial()->getTexturePath();

        if (texturepath != oldTexture) {
            submeshes[submesh]->getMaterial()->setTexturePath(texturepath);
            if (loaded) {
                textureLoad();
            }
        }
    }
}

std::string Mesh::getTexture(int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        return submeshes[submesh]->getMaterial()->getTexturePath();
    }

    return "";
}

Material* Mesh::getMaterial(int submesh){
    if (submesh >= 0 && submesh < submeshes.size()) {
        return submeshes[submesh]->getMaterial();
    }

    return NULL;
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

        render->setProgramShader(S_SHADER_MESH);

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->dynamic = dynamic;
            if (submeshes.size() == 1){
                //Use the same render for submesh
                submeshes[i]->setSubmeshRender(render);
            }else{
                submeshes[i]->getSubmeshRender()->setParent(render);
            }
            submeshes[i]->getSubmeshRender()->setPrimitiveType(primitiveType);
            submeshes[i]->renderLoad(shadow);
        }

    } else {

        instanciateShadowRender();

        shadowRender->setProgramShader(S_SHADER_DEPTH_RTT);

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->dynamic = dynamic;
            if (submeshes.size() == 1) {
                //Use the same render for submesh
                submeshes[i]->setSubmeshShadowRender(shadowRender);
            } else {
                submeshes[i]->getSubmeshShadowRender()->setParent(shadowRender);
            }
            submeshes[i]->getSubmeshShadowRender()->setPrimitiveType(primitiveType);
            submeshes[i]->renderLoad(shadow);
        }

    }

    return GraphicObject::renderLoad(shadow);
}

bool Mesh::load(){
    for (unsigned int i = 0; i < submeshes.size(); i++){
        if (submeshes.at(i)->getMaterial()->isTransparent()) {
            transparent = true;
        }
    }

    if (scene && scene->isLoadedShadow()) {
        renderLoad(true);
    }

    renderLoad(false);

    return GraphicObject::load();
}

bool Mesh::renderDraw(bool shadow){
    if (!GraphicObject::renderDraw(shadow))
        return false;

    if (!shadow) {

        render->prepareDraw();

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->renderDraw(shadow);
        }

        render->finishDraw();

    }else{

        if (!visible)
            return false;

        shadowRender->prepareDraw();

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->renderDraw(shadow);
        }

        shadowRender->finishDraw();

    }

    return true;
}

void Mesh::destroy(){

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }

    GraphicObject::destroy();
}
