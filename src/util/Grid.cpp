#include "Grid.h"
#include "AGLM.h"

using namespace glm;
using namespace std;
using namespace agl;

Renderer Grid::theRenderer;

Renderer& Grid::GetRenderer()
{
   return theRenderer;
}

Gird::Grid()
{
   mBlendMode = ADD;
   mTexture = -1;
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(int size, glm::vec3 _cameraPos)
{
   cameraPos = _cameraPos;
   if (!theRenderer.initialized())
   {
      theRenderer.init("../shaders/billboard.vs", "../shaders/billboard.fs");
   }
   createParticles(size);
}

void ParticleSystem::draw() 
{
   theRenderer.begin(mTexture, mBlendMode);
   for (int i = 0; i < mParticles.size(); i++)
   {
      Particle particle = mParticles[i];
      theRenderer.quad(particle.pos, particle.color, particle.size);
   }
}
