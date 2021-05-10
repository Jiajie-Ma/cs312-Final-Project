#ifndef Grid_H_
#define Grid_H_

#include "AGL.h"
#include "AGLM.h"
#include "renderer.h"
#include <string>
#include <vector>

namespace agl {
    class Grid
    {
    public:
        Grid();
        Grid(float size, float dim);
        virtual ~Grid();

        void draw();

   protected:
        float gridSize; // side length of the grid
        float gridDim; // number of vertices per side
        float nVertices; // number of vertices in total
        static Renderer theRenderer;
        GLuint mTexture;
        BlendMode mBlendMode;
        bool mInitialized;
   };
}

#endif
