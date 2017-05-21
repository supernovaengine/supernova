#include "Text.h"

#include "PrimitiveMode.h"
#include <string>
#include "render/TextureManager.h"
#include "platform/Log.h"
#include "STBText.h"

Text::Text(): Mesh2D() {
    primitiveMode = S_TRIANGLES;
    dynamic = true;
    stbtext = new STBText();
    text = "";
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

void Text::setFont(std::string font){
    this->font = font;
    setTexture(font);
}

void Text::setText(std::string text){
    this->text = text;
    if (loaded){
        drawText();
        renderManager.getRender()->updateVertices();
        renderManager.getRender()->updateTexcoords();
        renderManager.getRender()->updateNormals();
        renderManager.getRender()->updateIndices();
    }
}

void Text::setSize(int width, int height){
    Log::Error(LOG_TAG, "Can't set size of text");
}

void Text::drawText(){
    vertices.clear();
    texcoords.clear();
    normals.clear();
    std::vector<unsigned int> indices;
    
    int textWidth, textHeight;
    
    stbtext->createText(text, &vertices, &normals, &texcoords, &indices, &textWidth, &textHeight);
    
    this->width = textWidth;
    this->height = textHeight;
    
    submeshes[0]->setIndices(indices);
}

bool Text::load(){

    stbtext->load(font);
    
    drawText();
    
    return Mesh2D::load();
}
