#include "Text.h"

#include "render/ObjectRender.h"
#include <string>
#include "Log.h"
#include "util/STBText.h"

using namespace Supernova;

Text::Text(): Mesh2D() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
    dynamic = true;
    textmesh = true;
    stbtext = new STBText();
    text = "";
    fontSize = 40;
    multiline = true;
    
    userDefinedWidth = false;
    userDefinedHeight = false;

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);

    setMinBufferSize(50);
}

Text::Text(std::string font): Text() {
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

void Text::setFont(std::string font){
    this->font = font;
    setTexture(font + std::to_string('-') + std::to_string(fontSize));
    //Its not necessary to reload, setTexture do it
}

void Text::setFontSize(unsigned int fontSize){
    this->fontSize = fontSize;
    setTexture(font + std::to_string('-') + std::to_string(fontSize));
    //Its not necessary to reload, setTexture do it
}

void Text::setText(std::string text){
    if (this->text != text){
        this->text = text;
        if (loaded){
            if (buffer.getCount() > 0) {
                createText();
                updateBuffers();
            }else{
                load();
            }
        }
    }
}

void Text::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    userDefinedWidth = true;
    userDefinedHeight = true;
    if (loaded) {
        createText();
        updateBuffers();
    }
}

void Text::setWidth(int width){
    Mesh2D::setSize(width, height);
    userDefinedWidth = true;
    if (loaded) {
        createText();
        updateBuffers();
    }
}

void Text::setHeight(int height){
    Mesh2D::setSize(width, height);
    userDefinedHeight = true;
    if (loaded) {
        createText();
        updateBuffers();
    }
}

void Text::setInvertTexture(bool invertTexture){
    Mesh2D::setInvertTexture(invertTexture);
    if (loaded) {
        createText();
        updateBuffers();
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

std::string Text::getFont(){
    return font;
}

std::string Text::getText(){
    return text;
}

unsigned int Text::getNumChars(){
    return charPositions.size();
}

Vector2 Text::getCharPosition(unsigned int index){
    if (index < charPositions.size()){
        return charPositions[index];
    }

    return Vector2(-1, -1);
}

void Text::setMultiline(bool multiline){
    this->multiline = multiline;
}

void Text::createText(){
    buffer.clear();

    std::vector<unsigned int> indices_array;
    
    stbtext->createText(text, &buffer, indices_array, charPositions, width, height, userDefinedWidth, userDefinedHeight, multiline, invertTexture);

    indices.setValues(
            0, indices.getAttribute(S_INDEXATTRIBUTE),
            indices_array.size(), (char*)&indices_array[0], sizeof(unsigned int));

    submeshes[0]->setIndices("indices", indices_array.size());
}

bool Text::load(){
    if (stbtext->load(font.c_str(), fontSize, submeshes[0]->getMaterial()->getTexture())) {
        createText();
    }else{
        Log::Error("Can`t load font");
    }
    
    return Mesh2D::load();
}
