#ifndef shape_h
#define shape_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"
#include "render/MeshRender.h"

class Shape: public Mesh {

public:
    Shape();
	virtual ~Shape();

	bool load ();

    void generateTexcoords();

	void addVertex(Vector3 vertex);
    void addVertex(float x, float y);
};

#endif /* shape_h */
