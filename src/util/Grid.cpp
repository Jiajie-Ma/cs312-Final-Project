#include "grid.h"
#include "AGLM.h"

using namespace glm;
using namespace std;
using namespace agl;

Grid::Grid()
{
   mInitialized = false;
}

Grid::~Grid()
{
}

void Grid::init(float l, float w, int lR, int wR)
{
   gridLength = l;
   gridWidth = w;
   lRes = lR;
   wRes = wR;
   nVertices = lRes * wRes;
   
   nQuads = (lRes-1) * (wRes-1) * 2; // number of triangles in total
   vertices = new float[3 * nVertices]; // list of vertices
   quads = new unsigned int[nQuads * 3]; // list of faces

   for (int i = 0; i < lRes; i++){
      for (int j = 0; j < wRes; j++){
         GLfloat x = j/(float)(wRes-1);
         GLfloat y = i/(float)(lRes-1);
         GLfloat z = 0.0f;
         x = x * gridWidth - (gridWidth/2.0f);
         y = y * gridLength - (gridLength/2.0f);

         vertices[3 * i * wRes + 3 * j] = x;
         vertices[3 * i * wRes + 3 * j + 1] = y;
         vertices[3 * i * wRes + 3 * j + 2] = z;

         if (i+1 != wRes && j+1 < lRes){
            quads[6 * i * (wRes-1) + 6 * j] = i * wRes + j;
            quads[6 * i * (wRes-1) + 6 * j + 1] = i * wRes + j + wRes;
            quads[6 * i * (wRes-1) + 6 * j + 2] = i * wRes + j + wRes + 1;
            quads[6 * i * (wRes-1) + 6 * j + 3] = i * wRes + j;
            quads[6 * i * (wRes-1) + 6 * j + 4] = i * wRes + j + wRes + 1;
            quads[6 * i * (wRes-1) + 6 * j + 5] = i * wRes + j + 1;
         }
      }
   }

   mInitialized = true;

}

void Grid::uploadToGPU(GLuint pid) 
{
   assert(mInitialized);

   glUniform1f(glGetUniformLocation(pid, "gridlength"), gridLength);
   glUniform1f(glGetUniformLocation(pid, "gridwidth"), gridWidth);
   glUniform1f(glGetUniformLocation(pid, "lRes"), lRes);
   glUniform1f(glGetUniformLocation(pid, "wRes"), wRes);
}

int Grid::numVertices() const
{
   return nVertices;
}

int Grid::numTriangles() const
{
   return nQuads;
}

float* Grid::positions() const
{
   return vertices;
}

unsigned int* Grid::indices() const
{
   return quads;
}
