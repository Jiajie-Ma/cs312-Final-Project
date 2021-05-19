#ifndef Transformations_H_
#define Transformations_H_

#include "AGL.h"
#include "AGLM.h"

namespace agl {
   class Transformations
   {
   public:
    Transformations();
    ~Transformations();

    void init(const mat4& lookAt, const mat4& lookAt_mirrored, const vec3& light_position, float win_h, float win_w, float fovRadians);
    void updateTransformations(const mat4& lookAt, const mat4& lookAt_mirrored, const vec3& light_position);
    void updateProjection(float fovRadians, float aspect, float near, float far); 
    void updateShadowMVP(const vec3& light_position);
    void updateCamPos(const vec3& new_pos);
    void uploadToGPU(GLuint pid);

    mat4 getMVP() const;
    mat4 getMV() const;
    vec3 getCamPos() const;
    mat4 getProjection() const;
    mat4 getView() const;

   protected:
    int window_w;
    int window_h;
    vec3 cam_pos;
    float fov;

    mat4 mViewMatrix;
    mat4 mViewMatrix_mirrored;
    mat4 mProjectionMatrix;
    mat4 MVP;
    mat4 MV;
    mat3 NMV;
    mat4 inv_MV;
    mat4 inv_MVP;
    mat4 light_MVP;
    mat4 MVP_mirrored;
    mat4 MV_mirrored;
    mat4 inv_MV_mirrored;
    mat4 inv_MVP_mirrored;

    vec3 mRight;
    vec3 light_pos;
    
    float time;
    bool mInitialized;
   };
}

#endif
