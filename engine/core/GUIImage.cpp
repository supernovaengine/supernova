
#include "GUIImage.h"
#include "render/ObjectRender.h"
#include <string>
#include "image/TextureLoader.h"

using namespace Supernova;

GUIImage::GUIImage(): GUIObject(){
    primitiveType = S_PRIMITIVE_TRIANGLES;
    
    texWidth = 0;
    texHeight = 0;
    
    border_left = 0;
    border_right = 0;
    border_top = 0;
    border_bottom = 0;
}

GUIImage::~GUIImage(){

}

void GUIImage::setSize(int width, int height){
    Mesh2D::setSize(width, height);
    if (loaded) {
        createVertices();
        updateVertices();
    }
}

void GUIImage::setBorder(int border){
    border_left = border;
    border_right = border;
    border_top = border;
    border_bottom = border;
}

void GUIImage::createVertices(){
    
    vertices.clear();
    
    vertices.push_back(Vector3(0, 0, 0)); //0
    vertices.push_back(Vector3(width, 0, 0)); //1
    vertices.push_back(Vector3(width,  height, 0)); //2
    vertices.push_back(Vector3(0,  height, 0)); //3
    
    vertices.push_back(Vector3(border_left, border_top, 0)); //4
    vertices.push_back(Vector3(width-border_right, border_top, 0)); //5
    vertices.push_back(Vector3(width-border_right,  height-border_bottom, 0)); //6
    vertices.push_back(Vector3(border_left,  height-border_bottom, 0)); //7
    
    vertices.push_back(Vector3(border_left, 0, 0)); //8
    vertices.push_back(Vector3(0, border_top, 0)); //9
    
    vertices.push_back(Vector3(width-border_right, 0, 0)); //10
    vertices.push_back(Vector3(width, border_top, 0)); //11
    
    vertices.push_back(Vector3(width-border_right, height, 0)); //12
    vertices.push_back(Vector3(width, height-border_bottom, 0)); //13
    
    vertices.push_back(Vector3(border_left, height, 0)); //14
    vertices.push_back(Vector3(0, height-border_bottom, 0)); //15
    
    texcoords.clear();
    
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    
    texcoords.push_back(Vector2(border_left/(float)texWidth, border_top/(float)texHeight));
    texcoords.push_back(Vector2(1.0f-(border_right/(float)texWidth), border_top/(float)texHeight));
    texcoords.push_back(Vector2(1.0f-(border_right/(float)texWidth), 1.0f-(border_bottom/(float)texHeight)));
    texcoords.push_back(Vector2(border_left/(float)texWidth, 1.0f-(border_bottom/(float)texHeight)));
    
    texcoords.push_back(Vector2(border_left/(float)texWidth, 0));
    texcoords.push_back(Vector2(0, border_top/(float)texHeight));
    
    texcoords.push_back(Vector2(1.0f-(border_right/(float)texWidth), 0));
    texcoords.push_back(Vector2(1.0f, border_top/(float)texHeight));
    
    texcoords.push_back(Vector2(1.0f-(border_right/(float)texWidth), 1.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f-(border_bottom/(float)texHeight)));
    
    texcoords.push_back(Vector2((border_left/(float)texWidth), 1.0f));
    texcoords.push_back(Vector2(0, 1.0f-(border_bottom/(float)texHeight)));

    
    if (invertTexture){
        for (int i = 0; i < texcoords.size(); i++){
            texcoords[i].y = 1 - texcoords[i].y;
        }
    }
    
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
    
    normals.clear();
    for (int i = 0; i < texcoords.size(); i++){
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
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
