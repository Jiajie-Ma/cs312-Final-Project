#include "light.h"

using namespace agl;
using namespace glm;
using namespace std;

Light::Light()
{
   ka = vec3(1.0f)*0.2f;
   kd = vec3(0.4f, 0.6f, 0.5f);
   ks = vec3(1.0f, 1.0f, 1.0f);
   l_shininess = 80.0f;
   l_pos = vec3(50.0f, 0.0f, 0.0f);
   l_color = vec3(1.0f, 1.0f, 1.0f);
   radius = 50.0f;
   speed = 0.7f;
}

Light::~Light()
{
}

void Light::uploadToGPU(GLuint pid)
{
    glUniform3f(glGetUniformLocation(pid, "l_s"), ks[0], ks[1], ks[2]);
    glUniform3f(glGetUniformLocation(pid, "l_d"), kd[0], kd[1], kd[2]);
    glUniform3f(glGetUniformLocation(pid, "l_a"), ka[0], kd[1], kd[2]);
    glUniform3f(glGetUniformLocation(pid, "light_pos"), l_pos[0], l_pos[1], l_pos[2]);
}

vec3 Light::getLightPos() const
{
   return l_pos;
}

void Light::updateLightPos(float t)
{
    float s = t * speed;
    float x = radius * cos(t);
    float z = radius * sin(t);
    l_pos = vec3(x, 0, z);
}