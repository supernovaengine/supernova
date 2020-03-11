
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

        void instanciateMaterial();
        
    protected:
        ObjectRender* render;
        ObjectRender* shadowRender;

        Material* material;

        std::map<std::string, Buffer*> buffers;
        std::string defaultBuffer;

        Rect scissor;

        Matrix4 normalMatrix;

        bool visible;
        bool transparent;
        float distanceToCamera;

        unsigned int minBufferSize;

        bool instanciateRender();
        bool instanciateShadowRender();

        void setSceneTransparency(bool transparency);

        void updateDistanceToCamera();

        void updateBuffer(std::string name);

        virtual bool textureLoad();

    public:

        GraphicObject();
        virtual ~GraphicObject();
        
        Matrix4 getNormalMatrix();

        void setVisible(bool visible);
        bool isVisible();

        unsigned int getMinBufferSize();

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        void setTexture(Texture* texture);
        void setTexture(std::string texturepath);
        std::string getTexture();

        Material* getMaterial();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();

        virtual bool renderLoad(bool shadow);
        virtual bool renderDraw(bool shadow);
        
        virtual bool draw();
        virtual bool load();

        virtual void destroy();

    };
    
}

#endif /* GraphicObject_h */
