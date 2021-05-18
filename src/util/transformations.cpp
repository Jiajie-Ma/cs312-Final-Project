#include "transformations.h"

using namespace agl;
using namespace glm;
using namespace std;

Transformations::Transformations()
{
    mInitialized = false;
}

Transformations::~Transformations()
{
}

void Transformations::init(const mat4& lookAt, const vec3& light_position, float win_h, float win_w, float fovRadians)
{
    window_h = win_h;
    window_w = win_w;
    mViewMatrix = lookAt;
    fov = fovRadians;
    light_pos = light_position;

    mProjectionMatrix = glm::perspective(fov, 1.0f, 0.1f, 100.0f);
    MV = mViewMatrix;
    MVP = mProjectionMatrix * mViewMatrix;
    NMV = glm::mat3(glm::vec3(MV[0]), glm::vec3(MV[1]), glm::vec3(MV[2]));

    mRight = vec3(0.0f, 0.0f, 1.0f);
    updateShadowMVP(light_pos);

    time = glfwGetTime();

    mInitialized = true;
}

void Transformations::updateTransformations(const mat4& lookAt, const vec3& light_position)
{
    assert(mInitialized);

    mViewMatrix = lookAt;
    light_pos = light_position;

    MV = mViewMatrix;
    MVP = mProjectionMatrix * mViewMatrix;
    NMV = glm::mat3(glm::vec3(MV[0]), glm::vec3(MV[1]), glm::vec3(MV[2]));
    
    updateShadowMVP(light_pos);

    time = glfwGetTime();
}

void Transformations::updateShadowMVP(const vec3& light_position)
{
    vec3 right_vector_ = vec3(0.0f, 0.0f, 1.0f); // light rotating around z axis
    vec3 look = vec3(0.0f);
    mat4 V = glm::lookAt(light_position, look, vec3(0.0f, 1.0f, 0.0f));
    mat4 P = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 20.0f, 120.0f);
    light_MVP = P*V;
}

void Transformations::updateProjection(float fovRadians, float aspect, float near, float far)
{
    assert(mInitialized);
    
    fov = fovRadians;
    mProjectionMatrix = glm::perspective(fovRadians, aspect, near, far);
}

void Transformations::uploadToGPU(GLuint pid)
{
    assert(mInitialized);

    glUniformMatrix4fv(glGetUniformLocation(pid, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(pid, "light_MVP"), 1, GL_FALSE, &light_MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(pid, "ModelViewMatrix"), 1, GL_FALSE, &MV[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(pid, "NormalMatrix"), 1, GL_FALSE, &NMV[0][0]);

    glUniform1i(glGetUniformLocation(pid, "window_w"), window_w);
    glUniform1i(glGetUniformLocation(pid, "window_h"), window_h);
    glUniform1f(glGetUniformLocation(pid, "time"), time);
}

mat4 Transformations::getMVP() const{

    assert(mInitialized);
    return MVP;
}
mat4 Transformations::getMV() const{

    assert(mInitialized);
    return MV;
}

mat4 Transformations::getProjection() const{

    assert(mInitialized);
    return mProjectionMatrix;
}

mat4 Transformations::getView() const{
    
    assert(mInitialized);
    return mViewMatrix;
}