
#include "UIImage.h"
#include "render/ObjectRender.h"
#include <string>
#include "image/TextureLoader.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

UIImage::UIImage(): UIObject(){
    primitiveType = S_PRIMITIVE_TRIANGLES;
    
    texWidth = 0;
    texHeight = 0;
    
    border_left = 0;
    border_right = 0;
    border_top = 0;
    border_bottom = 0;

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

UIImage::~UIImage(){

}

void UIImage::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        updateBuffers();
    }
}

void UIImage::setBorder(int border){
    border_left = border;
    border_right = border;
    border_top = border;
    border_bottom = border;

    setClipBorder(border_left, border_top, border_right, border_bottom);
}

void UIImage::createVertices(){

    buffer.clear();

    AttributeData* atrVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    
    buffer.addVector3(atrVertex, Vector3(0, 0, 0)); //0
    buffer.addVector3(atrVertex, Vector3(width, 0, 0)); //1
    buffer.addVector3(atrVertex, Vector3(width,  height, 0)); //2
    buffer.addVector3(atrVertex, Vector3(0,  height, 0)); //3
    
    buffer.addVector3(atrVertex, Vector3(border_left, border_top, 0)); //4
    buffer.addVector3(atrVertex, Vector3(width-border_right, border_top, 0)); //5
    buffer.addVector3(atrVertex, Vector3(width-border_right,  height-border_bottom, 0)); //6
    buffer.addVector3(atrVertex, Vector3(border_left,  height-border_bottom, 0)); //7
    
    buffer.addVector3(atrVertex, Vector3(border_left, 0, 0)); //8
    buffer.addVector3(atrVertex, Vector3(0, border_top, 0)); //9
    
    buffer.addVector3(atrVertex, Vector3(width-border_right, 0, 0)); //10
    buffer.addVector3(atrVertex, Vector3(width, border_top, 0)); //11
    
    buffer.addVector3(atrVertex, Vector3(width-border_right, height, 0)); //12
    buffer.addVector3(atrVertex, Vector3(width, height-border_bottom, 0)); //13
    
    buffer.addVector3(atrVertex, Vector3(border_left, height, 0)); //14
    buffer.addVector3(atrVertex, Vector3(0, height-border_bottom, 0)); //15

    AttributeData* atrTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    
    buffer.addVector2(atrTexcoord, Vector2(0.0f, convTex(0.0f)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, convTex(0.0f)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, convTex(1.0f)));
    buffer.addVector2(atrTexcoord, Vector2(0.0f, convTex(1.0f)));
    
    buffer.addVector2(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(border_top/(float)texHeight)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(border_top/(float)texHeight)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(1.0f-(border_bottom/(float)texHeight))));
    buffer.addVector2(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(1.0f-(border_bottom/(float)texHeight))));
    
    buffer.addVector2(atrTexcoord, Vector2(border_left/(float)texWidth, convTex(0)));
    buffer.addVector2(atrTexcoord, Vector2(0, convTex(border_top/(float)texHeight)));
    
    buffer.addVector2(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(0)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, convTex(border_top/(float)texHeight)));
    
    buffer.addVector2(atrTexcoord, Vector2(1.0f-(border_right/(float)texWidth), convTex(1.0f)));
    buffer.addVector2(atrTexcoord, Vector2(1.0f, convTex(1.0f-(border_bottom/(float)texHeight))));
    
    buffer.addVector2(atrTexcoord, Vector2((border_left/(float)texWidth), convTex(1.0f)));
    buffer.addVector2(atrTexcoord, Vector2(0, convTex(1.0f-(border_bottom/(float)texHeight))));

    
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

    indices.setValues(
            0, indices.getAttribute(S_INDEXATTRIBUTE),
            54, (char*)&indices_array[0], sizeof(unsigned int));

    submeshes[0]->setIndices("indices", 54);

    AttributeData* atrNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    for (int i = 0; i < buffer.getCount(); i++){
        buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
    }
}

bool UIImage::load(){
    
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

    return UIObject::load();
}
