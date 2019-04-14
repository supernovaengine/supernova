
#ifndef GraphicObject_h
#define GraphicObject_h

//
// (c) 2018 Eduardo Doria.
//

#include "Object.h"
#include "Material.h"
#include "buffer/Buffer.h"
#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"
#include "render/ObjectRender.h"

namespace Supernova {

    class GraphicObject: public Object {

    private:

        void setSceneTransparency(bool transparency);
        
    protected:
        ObjectRender* render;
        ObjectRender* shadowRender;

        std::map<std::string, Buffer*> buffers;
        std::string defaultBuffer;

        Rect scissor;

        Material material;

        Matrix4 normalMatrix;

        bool visible;
        bool transparent;
        float distanceToCamera;

        unsigned int minBufferSize;

        void updateDistanceToCamera();

        void updateBuffer(std::string name);
        void prepareShadowRender();
        void prepareRender();

    public:

        GraphicObject();
        virtual ~GraphicObject();
        
        Matrix4 getNormalMatrix();

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();
        
        void setTexture(Texture* texture);
        void setTexture(std::string texturepath);
        std::string getTexture();

        void setVisible(bool visible);
        bool isVisible();
        
        Material* getMaterial();

        unsigned int getMinBufferSize();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();
        
        virtual bool textureLoad();
        virtual bool shadowLoad();
        virtual bool shadowDraw();
        
        virtual bool renderDraw();
        
        virtual bool draw();
        virtual bool load();

        virtual void destroy();

    };
    
}

#endif /* GraphicObject_h */
