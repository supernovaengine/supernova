
#ifndef POINTS_H
#define POINTS_H

#define S_POINTSIZE_PIXEL 0
#define S_POINTSIZE_WIDTH 1
#define S_POINTSIZE_HEIGHT 2

#include "GraphicObject.h"
#include "render/ObjectRender.h"
#include <map>

namespace Supernova {

    class Points: public GraphicObject {

    private:
        InterleavedBuffer buffer;

        Material material;

        int texWidth;
        int texHeight;

        float pointScale;

        int pointSizeReference;

        void updatePointScale();
        bool sortPoints();
        bool shouldSort();
        std::vector<int> findFramesByString(std::string id);

    protected:

        struct FramesData{
            std::string id;
            Rect rect;
        };

        struct Point{
            Vector3 position;
            Vector3 normal;
            Rect textureRect;
            float size;
            Vector4 color;
            float rotation;
            bool visible;
        };

        std::vector<Point> points;
        std::vector<Point> sortedPoints;

        bool sizeAttenuation;
        float pointScaleFactor;
        float minPointSize;
        float maxPointSize;

        bool useTextureRects;

        bool automaticUpdate;
        bool pertmitSortTransparentPoints;

        std::map<int,FramesData> framesRect;

        void copyBuffer();
        void updatePoints();
        void normalizeTextureRects();

    public:
        Points();
        virtual ~Points();

        virtual void setColor(Vector4 color);
        virtual void setColor(float red, float green, float blue, float alpha);
        virtual Vector4 getColor();

        void setTexture(Texture* texture);
        void setTexture(std::string texturepath);
        std::string getTexture();

        Material* getMaterial();

        void setSizeAttenuation(bool sizeAttenuation);
        void setPointScaleFactor(float pointScaleFactor);
        void setPointSizeReference(int pointSizeReference);
        void setMinPointSize(float minPointSize);
        void setMaxPointSize(float maxPointSize);

        virtual void addPoint();
        virtual void addPoint(Vector3 position);
        virtual void clearPoints();

        void setPointPosition(int point, Vector3 position);
        void setPointPosition(int point, float x, float y, float z);
        void setPointSize(int point, float size);
        void setPointColor(int point, Vector4 color);
        void setPointColor(int point, float red, float green, float blue, float alpha);
        void setPointRotation(int point, float rotation);
        void setPointSprite(int point, int id);
        void setPointSprite(int point, std::string name);
        void setPointVisible(int point, bool visible);
        
        Vector3 getPointPosition(int point);
        float getPointSize(int point);
        Vector4 getPointColor(int point);
        float getPointRotation(int point);

        void addSpriteFrame(int id, std::string name, Rect rect);
        void addSpriteFrame(std::string name, float x, float y, float width, float height);
        void addSpriteFrame(float x, float y, float width, float height);
        void addSpriteFrame(Rect rect);
        void removeSpriteFrame(int id);
        void removeSpriteFrame(std::string name);

        void setPertmitSortTransparentPoints(bool pertmitSortTransparentPoints);
        bool isPertmitSortTransparentPoints();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();
        
        virtual bool renderDraw(bool shadow);
        
        virtual bool textureLoad();
        
        virtual bool load();
    };
    
}


#endif
