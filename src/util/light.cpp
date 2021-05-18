#include "light.h"

using namespace agl;
using namespace glm;
using namespace std;

Light::Light()
{
   ka = vec3(0.1f, 0.1f, 0.1f);
   kd = vec3(0.4f, 0.6f, 1.0f);
   ks = vec3(1.0f, 1.0f, 1.0f);
   l_shininess = 80.0f;
   l_pos = vec3(50.0f, 0.0f, 0.0f);
   l_color = vec3(1.0f, 1.0f, 1.0f);
   radius = 50.0f;
   speed = 0.07f;
}

Light::~Light()
{
}

void Light::draw(GLuint pid)
{
    glUniform3f(glGetUniformLocation(pid, "Material.Ks"), ks[0], ks[1], ks[2]);
    glUniform3f(glGetUniformLocation(pid, "Material.Kd"), kd[0], kd[1], kd[2]);
    glUniform3f(glGetUniformLocation(pid, "Material.Ka"), ka[0], kd[1], kd[2]);
    glUniform1f(glGetUniformLocation(pid, "Material.shininess"), l_shininess);
    glUniform4f(glGetUniformLocation(pid, "Light.position"), l_pos[0], l_pos[1], l_pos[2], l_pos[3]);
    glUniform3f(glGetUniformLocation(pid, "Light.color"), l_color[0], l_color[1], l_color[2]);
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