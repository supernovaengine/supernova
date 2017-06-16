

#ifndef SkyBox_h
#define SkyBox_h

#include <vector>
#include <string>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class SkyBox: public Mesh {
    private:
        //To use texture cube
        std::string textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown;
        Matrix4 skyViewProjectionMatrix;
        Matrix4 skyViewMatrix;

    protected:
        virtual void updateMVPMatrix();
        
    public:
        SkyBox();
        SkyBox(float width, float height, float depth);
        virtual ~SkyBox();
        
        void setTextureFront(std::string texture);
        void setTextureBack(std::string texture);
        void setTextureLeft(std::string texture);
        void setTextureRight(std::string texture);
        void setTextureUp(std::string texture);
        void setTextureDown(std::string texture);

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        
        virtual bool load();
        virtual void update();
        
    };
    
}


#endif /* SkyBox_h */
