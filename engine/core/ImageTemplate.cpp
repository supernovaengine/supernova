
#include "ImageTemplate.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"

ImageTemplate::ImageTemplate(): Mesh2D(){
}

ImageTemplate::~ImageTemplate(){

}

bool ImageTemplate::load(){

    if (submeshes[0].getTexture() != ""){
        
        int cropWidth = 0;
        int cropHeight = 0;
        
        if (!loaded || (rawImageWidth == 0 && rawImageHeight == 0)){
        
            TextureLoader image(submeshes[0].getTexture());

            //TextureManager::loadTexture(image.getRawImage(), submeshes[0].getTexture());
            
            rawImageWidth = image.getRawImage()->getWidth();
            rawImageHeight = image.getRawImage()->getWidth();
            
            cropWidth = rawImageWidth / 3;
            cropHeight = rawImageWidth / 3;
            
            for (int x = 0; x < 3; x++){
                for (int y = 0; y < 3; y++){
                    
                    TextureFile* cropedTex = new TextureFile(*image.getRawImage());
                    cropedTex->crop(x * cropWidth, y * cropHeight, cropWidth, cropHeight);
                    TextureManager::loadTexture(cropedTex, getTexture()+std::to_string(x)+"-"+std::to_string(y));
                    
                    partImage.push_back(new Image());
                    partImage[partImage.size()-1]->setTexture(getTexture()+std::to_string(x)+"-"+std::to_string(y));
                    this->addObject(partImage[partImage.size()-1]);
                }
            }
            
        }
        
        if (cropWidth == 0 && cropHeight == 0){
            cropWidth = rawImageWidth / 3;
            cropHeight = rawImageWidth / 3;
        }

        if (width == 0 && height == 0){
            width = rawImageWidth;
            height = rawImageHeight;
        }

        if (width == 0)
            width = rawImageWidth;
        if (height == 0)
            height = rawImageWidth;

        if (width < (cropWidth * 2)){
            cropWidth = width / 2;
        }
        if (height < (cropHeight * 2)){
            cropHeight = height / 2;
        }

        float sizeX = width - cropWidth * 3;
        float sizeY = height - cropHeight * 3;

        int tileWidth, tileHeight;
        float offX, offY;

        int partIndex = 0;
        for (int x = 0; x < 3; x++){
            for (int y = 0; y < 3; y++){

                offX = cropWidth;
                offY = cropHeight;

                tileWidth = cropWidth;
                tileHeight = cropHeight;

                if (x == 1)
                    tileWidth += sizeX;
                if (y == 1)
                    tileHeight += sizeY;
                if (x == 2)
                    offX += sizeX/2;
                if (y == 2)
                    offY += sizeY/2;

                float texCoordX = (float)tileWidth/cropWidth;
                float texCoordY = (float)tileHeight/cropHeight;

                std::vector<Vector2> texcoords;

                texcoords.push_back(Vector2(0.0f, 0.0f));
                texcoords.push_back(Vector2(texCoordX, 0.0f));
                texcoords.push_back(Vector2(texCoordX, texCoordY));
                texcoords.push_back(Vector2(0.0f, texCoordY));

                partImage[partIndex]->setTexcoords(texcoords);
                partImage[partIndex]->setSize(tileWidth, tileHeight);
                partImage[partIndex]->setPosition(offX*x, offY*y, 0);

                partIndex++;
            }
        }
    }

    return Mesh2D::load();
}
