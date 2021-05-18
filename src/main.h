// Haverford College, Jiajie Ma, 2021

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#include "util/AGL.h"
#include "util/AGLM.h"
#include "util/framebuffer.h"
#include "util/camera.h"
#include "util/transformations.h"
#include "util/light.h"

#include "noise/noise.h"
#include "terrain/terrain.h"

using namespace std;
using namespace glm;
using namespace agl;

// globals
int window_width_ = 1920;
int window_height_ = 1080;
int tex_size = 2048;


Terrain terrain_;
Noise noise_;
Camera cam_;
Light light_;
Transformations T_;

GLFWwindow* window;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   if (key == GLFW_KEY_ESCAPE)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if (height == 0) height = 1;

   // Set Viewport to window dimensions
   glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

static void initWindow(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static GLFWwindow* openWindow(string title){
    const char* c = title.c_str();
    window_ = glfwCreateWindow(window_width_, window_height_, c, NULL, NULL);
    if( !window_ ){
        glfwTerminate();
        return -1;
    }
    return window_;
}

static void initGlew(){
    /// GLEW Initialization
    if( glewInit() != GLEW_OK ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
    glGetError();
}

static void closeWindow(GLFWwindow* window){
    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}