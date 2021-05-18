#ifndef Camera_H_
#define Camera_H_

#include "AGL.h"
#include "AGLM.h"

namespace agl {
   class Camera
   {
   public:
      Camera();
      ~Camera();

      void lookAt(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& up);
      void updateCamPos(const vec3& new_pos);
      void updateViewMatrix();

      glm::vec3 getPosition() const;
      glm::vec3 getLook() const;
      glm::vec3 getUp() const;
      glm::vec3 getRight() const;
      glm::vec3 getDirection() const;
      glm::mat4 getViewMatrix() const;

      void uploadToGPU(GLuint pid);

   protected:
      vec3 mPos;
      vec3 mUp;
      vec3 mLook;
      vec3 mRight;
      vec3 cam_dir;
      mat4 mViewMatrix;
   };
}

#endif
