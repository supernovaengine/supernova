#include "Text.h"

#include "PrimitiveMode.h"
#include <string>
#include "platform/Log.h"
#include "STBText.h"

using namespace Supernova;

Text::Text(): Mesh2D() {
    primitiveMode = S_TRIANGLES;
    dynamic = true;
    textmesh = true;
    stbtext = new STBText();
    text = "";
    fontSize = 40;
    multiline = true;
    
    userDefinedWidth = false;
    userDefinedHeight = false;

    setMinBufferSize(50);
}

Text::Text(const char* font): Text() {
    setFont(font);
}

Text::~Text() {
    delete stbtext;
}

Text& Text::operator = ( const char* v ){
    setText(v);
    
    return *this;
}

bool Text::operator == ( const char* v ) const{
    return ( text == v );
}

bool Text::operator != ( const char* v ) const{
    return ( text != v );
}

void Text::setMinBufferSize(unsigned int characters){
    this->minBufferSize = characters * 4;
    this->submeshes[0]->minBufferSize = characters * 6;
}

void Text::setFont(const char* font){
    this->font = font;
    setTexture(font + std::to_string('-') + std::to_string(fontSize));
    //Its not necessary to reload, setTexture do it
}

void Text::setFontSize(unsigned int fontSize){
    this->fontSize = fontSize;
    setTexture(font + std::to_string('-') + std::to_string(fontSize));
    //Its not necessary to reload, setTexture do it
}

void Text::setText(const char* text){
    this->text = text;
    if (loaded){
        createText();
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, normals.size());
        render->updateIndex(submeshes[0]->getIndices()->size());
    }
}

void Text::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    userDefinedWidth = true;
    userDefinedHeight = true;
    if (loaded) {
        createText();
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size());
    }
}

void Text::setWidth(int width){
    Mesh2D::setSize(width, height);
    userDefinedWidth = true;
    if (loaded) {
        createText();
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size());
    }
}

void Text::setHeight(int height){
    Mesh2D::setSize(width, height);
    userDefinedHeight = true;
    if (loaded) {
        createText();
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size());
    }
}

void Text::setInvertTexture(bool invertTexture){
    Mesh2D::setInvertTexture(invertTexture);
    if (loaded) {
        createText();
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size());
        render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size());
    }
}

float Text::getAscent(){
    if (!stbtext)
        return 0;
    else
        return stbtext->getAscent();
}

float Text::getDescent(){
    if (!stbtext)
        return 0;
    else
        return stbtext->getDescent();
}

float Text::getLineGap(){
    if (!stbtext)
        return 0;
    else
        return stbtext->getLineGap();
}

int Text::getLineHeight(){
    if (!stbtext)
        return 0;
    else
        return stbtext->getLineHeight();
}

void Text::setMultiline(bool multiline){
    this->multiline = multiline;
}

void Text::createText(){
    vertices.clear();
    texcoords.clear();
    normals.clear();
    std::vector<unsigned int> indices;
    
    stbtext->createText(text, &vertices, &normals, &texcoords, &indices, &width, &height, userDefinedWidth, userDefinedHeight, multiline, invertTexture);
    
    submeshes[0]->setIndices(indices);
}

bool Text::load(){

    stbtext->load(font.c_str(), fontSize, submeshes[0]->getMaterial()->getTexture());
    setInvertTexture(isIn3DScene());
    createText();
    
    return Mesh2D::load();
}
