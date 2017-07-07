#ifndef PointRender_h
#define PointRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "math/Rect.h"
#include "Material.h"
#include "Texture.h"
#include <vector>

namespace Supernova {

    class Points;

    class PointRender {
        
    private:
        
        void checkLighting();
        void checkFog();
        void checkTextureRect();
        void fillPointProperties();
        
    protected:
        
        bool lighting;
        bool hasfog;
        bool hasTextureRect;
        
        Points* points;
        
        int numPoints;
        bool textured;

        //-------begin points properties-------
        SceneRender* sceneRender;
        
        std::vector<Vector3>* positions;
        std::vector<Vector3>* normals;
        std::vector<Rect*>* textureRects;
        std::vector<float>* pointSizes;
        std::vector<Vector4>* pointColors;
        
        Matrix4 modelMatrix;
        Matrix4 normalMatrix;
        Matrix4 modelViewProjectionMatrix;
        Vector3 cameraPosition;
        
        Texture* texture;

        unsigned int minBufferSize;
        //------------end------------

        PointRender();

    public:

        virtual ~PointRender();

        static void newInstance(PointRender** render);
        
        void setPoints(Points* points);

        virtual void updatePositions();
        virtual void updateNormals();
        virtual void updatePointSizes();
        virtual void updateTextureRects();
        virtual void updatePointColors();
        
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
        
    };
    
}


#endif /* PointRender_h */
