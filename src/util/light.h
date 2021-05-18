#ifndef Light_H_
#define Light_H_

#include "AGL.h"
#include "AGLM.h"
#include <string>
#include <vector>

namespace agl {
    class Light
    {
    public:
        Light();
        virtual ~Light();

        void draw(GLuint pid);
        void updateLightPos(float t);

        vec3 getLightPos() const;

   protected:
        vec3 ka;
        vec3 ks;
        vec3 kd;
        vec3 l_pos;
        vec3 l_color;
        float l_shininess;
        float radius; // radius of the light's trajectory
        float speed; // speed of the light
   };
}

#endif
