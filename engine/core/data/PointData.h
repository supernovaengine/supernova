#ifndef PointData_h
#define PointData_h

#include <vector>

namespace Supernova{
    
    class PointData{
        
    public:
        
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> textureRects;
        std::vector<float> sizes;
        std::vector<float> colors;
        
        PointData();
        virtual ~PointData();
        
        void clearPoints();
    };
    
}


#endif /* PointData_h */
