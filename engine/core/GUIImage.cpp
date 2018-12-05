
#include "GUIImage.h"
#include "render/ObjectRender.h"
#include <string>
#include "image/TextureLoader.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

GUIImage::GUIImage(): GUIObject(){
    primitiveType = S_PRIMITIVE_TRIANGLES;
    
    texWidth = 0;
    texHeight = 0;
    
    border_left = 0;
    border_right = 0;
    border_top = 0;
    border_bottom = 0;

    buffers[0].clearAll();
    buffers[0].setName("vertices");
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffers[0].addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

GUIImage::~GUIImage(){

}

void GUIImage::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        updateBuffers();
    }
}

void GUIImage::setBorder(int border){
    border_left = border;
    border_right = border;
    border_top = border;
    border_bottom = border;

    setClipBorder(border_left, border_top, border_right, border_bottom);
}

void GUIImage::createVertices(){

    buffers[0].clearBuffer();

    AttributeData* atrVertex = buffers[0].getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    
    buffers[0].addValue(atrVertex, Vector3(0, 0, 0)); //0
    buffers[0].addValue(atrVertex, Vector3(width, 0, 0)); //1
    buffers[0].addValue(atrVertex, Vector3(width,  height, 0)); //2
    buffers[0].addValue(atrVertex, Vector3(0,  height, 0)); //3
    
    buffers[0].addValue(atrVertex, Vector3(border_left, border_top, 0)); //4
    buffers[0].addValue(atrVertex, Vector3(width-border_right, border_top, 0)); //5
    buffers[0].addValue(atrVertex, Vector3(width-border_right,  height-border_bottom, 0)); //6
    buffers[0].addValue(atrVertex, Vector3(border_left,  height-border_bottom, 0)); //7
    
    buffers[0].addValue(atrVertex, Vector3(border_left, 0, 0)); //8
    buffers[0].addValue(atrVertex, Vector3(0, border_top, 0)); //9
    
    buffers[0].addValue(atrVertex, Vector3(width-border_right, 0, 0)); //10
    buffers[0].addValue(atrVertex, Vector3(width, border_top, 0)); //11
    
    buffers[0].addValue(atrVertex, Vector3(width-border_right, height, 0)); //12
    buffers[0].addValue(atrVertex, Vector3(width, height-border_bottom, 0)); //13
    
    buffers[0].addValue(atrVertex, Vector3(border_left, height, 0)); //14
    buffers[0].addValue(atrVertex, Vector3(0, height-border_bottom, 0)); //15

    AttributeData* atrTexcoord = buffers[0].getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    
    buffers[0].addValue(atrTexcoord, Vector2(0.0f, convTex(0.0f)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f, convTex(0.0f)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f, convTex(1.0f)));
    buffers[0].addValue(atrTexcoord, Vector2(0.0f, convTex(1.0f)));
    
    buffers[0].addValue(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(border_top/(float)texHeight)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(border_top/(float)texHeight)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(1.0f-(border_bottom/(float)texHeight))));
    buffers[0].addValue(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(1.0f-(border_bottom/(float)texHeight))));
    
    buffers[0].addValue(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(0)));
    buffers[0].addValue(atrTexcoord, Vector2(0, convTex(border_top/(float)texHeight)));
    
    buffers[0].addValue(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(0)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f, convTex(border_top/(float)texHeight)));
    
    buffers[0].addValue(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(1.0f)));
    buffers[0].addValue(atrTexcoord, Vector2(1.0f, convTex(1.0f-(border_bottom/(float)texHeight))));
    
    buffers[0].addValue(atrTexcoord, Vector2((border_left/(float)texWidth), convTex(1.0f)));
    buffers[0].addValue(atrTexcoord, Vector2(0, convTex(1.0f-(border_bottom/(float)texHeight))));

    
    static const unsigned int indices_array[] = {
        0,  8,  4,
        0,  4,  9,
        
        8, 10, 5,
        8, 5, 4,
        
        10, 1, 11,
        10, 11, 5,
        
        5, 11, 13,
        5, 13, 6,
        
        6, 13, 2,
        6, 2, 12,
        
        7, 6, 12,
        7, 12, 14,
        
        15, 7, 14,
        15, 14, 3,
        
        9, 4, 7,
        9, 7, 15,
        
        4,  5,  6,
        4,  6,  7
        
    };
    
    std::vector<unsigned int> indices;
    indices.assign(indices_array, std::end(indices_array));
    submeshes[0]->setIndices(indices);

    AttributeData* atrNormal = buffers[0].getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    for (int i = 0; i < buffers[0].getCount(); i++){
        buffers[0].addValue(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    }
}

bool GUIImage::load(){
    
    if (submeshes[0]->getMaterial()->getTexture()){
        submeshes[0]->getMaterial()->getTexture()->load();
        texWidth = submeshes[0]->getMaterial()->getTexture()->getWidth();
        texHeight = submeshes[0]->getMaterial()->getTexture()->getHeight();
    }
    if (this->width == 0 && this->height == 0){
        this->width = texWidth;
        this->height = texHeight;
    }
    
    setInvertTexture(isIn3DScene());
    createVertices();

    return GUIObject::load();
}
