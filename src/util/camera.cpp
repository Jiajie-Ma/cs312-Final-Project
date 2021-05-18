#include "camera.h"

using namespace agl;
using namespace glm;
using namespace std;

Camera::Camera()
{
   mPos = vec3(0.0f, 0.0f, 3.0f);
   mUp = vec3(0.0f, 1.0f, 0.0f);
   mLook = vec3(0.0f, 0.0f, 0.0f);
   cam_dir = vec3(mLook - mPos);
   mRight = glm::normalize(glm::cross(mLook, mUp));
   mViewMatrix = glm::lookAt(mPos, mLook, mUp);
}

Camera::~Camera()
{
}

void Camera::lookAt(const vec3& lookfrom, const vec3& lookat, const vec3& up)
{
   mPos= lookfrom;
   mLook = lookat;
   mUp = up;
   cam_dir = vec3(mLook - mPos);
   mRight = glm::normalize(glm::cross(mLook, mUp));

   mViewMatrix = glm::lookAt(mPos, mLook, mUp);
}

void Camera::updateCamPos(const vec3& new_pos){
    mPos = new_pos;
}

void Camera::updateViewMatrix(){
   mViewMatrix = glm::lookAt(mPos, mLook, mUp);
}

void Camera::uploadToGPU(GLuint pid)
{
   glUniform3f(glGetUniformLocation(pid, "camPos"), mPos[0], mPos[1], mPos[2]);
}

vec3 Camera::getPosition() const
{
   return mPos;
}

vec3 Camera::getLook() const
{
   return mLook;
}

vec3 Camera::getUp() const
{
   return mUp;
}

vec3 Camera::getRight() const
{
   return mRight;
}

vec3 Camera::getDirection() const
{
   return cam_dir;
}

mat4 Camera::getViewMatrix() const
{
   return mViewMatrix;
}