#ifndef Grid_H_
#define Grid_H_

#include "AGL.h"
#include "AGLM.h"
#include <string>
#include <vector>

namespace agl {
    class Grid
    {
    public:
        Grid();
        ~Grid();
        
        void init(float l, float w, int lR, int wR);
        void uploadToGPU(GLuint pid);
        
        int numVertices() const;
        int numTriangles() const;
        float* positions() const;
        unsigned int* indices() const;

   protected:
        float gridLength; // length of the grid
        float gridWidth; // width of the grid
        int lRes; // number of vertices on the length side
        int wRes; // number of vertices on the width side
        int nVertices; // number of vertices in total
        int nQuads; // number of triangles in tota
        float* vertices; // list of vertices
        unsigned int* quads; // list of faces
        bool mInitialized;
   };
}

#endif
