// Haverford College, Jiajie Ma, 2021

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#include "util/AGL.h"
#include "util/AGLM.h"
#include "util/camera.h"
#include "util/transformations.h"
#include "util/light.h"
#include "util/grid.h"
#include "util/image.h"
#include "util/noise.h"

using namespace std;
using namespace glm;
using namespace agl;

// globals
int window_width_ = 1920;
int window_height_ = 1080;
int tex_size = 2048;
float render_size = 25.0;

Camera cam_;
Light light_;
Transformations T_;
Grid grid_;

GLFWwindow* window;
unsigned int program_id_, vboPosId, elementbuffer, vaoId;

// control
const float cameraSpeed = 0.25f;
float lastX = 250.0f, lastY = 250.0f, dist = 50.0f, azimuth = 0.0f, elevation = 0.0f;
bool control = false, zoom = false;

//noise
int octaves = 8, mode = 1;
float H = 1.0f, f = 2.0f, height = 15.0f, density = 7.5f;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if(height == 0) height = 1;
	
   // Set Viewport to window dimensions
   glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);

   int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (state == GLFW_PRESS)
   {
      control = true;
      int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
      if (keyPress == GLFW_PRESS) {
         zoom = true;
      }

   }
   else if (state == GLFW_RELEASE)
   {
      control = false;
      zoom = false;
   }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
   if (control)
   {
      // zoom in/out with shift and cursor
      if (zoom){
         float distDelta = ypos - lastY;
         
         distDelta *= cameraSpeed;

         dist += distDelta;

         if (dist < 1.0f){
            dist = 1.0f;
         }
      }
      else{
         // rotate with cursor
         float xDelta = lastX - xpos;
         float yDelta = ypos - lastY;

         xDelta *= cameraSpeed;
         yDelta *= cameraSpeed;

         azimuth += xDelta;
         elevation += yDelta;

         if(elevation > 89.9f){
            elevation = 89.9f;
         }   
         else if(elevation < -89.9f){
            elevation = -89.9f;
         }   
      }
   }

   lastX = xpos;
   lastY = ypos;
}

static void initWindow(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static GLFWwindow* openWindow(string title){
    const char* c = title.c_str();
    window = glfwCreateWindow(window_width_, window_height_, c, NULL, NULL);
    if( !window ){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    return window;
}

static void initGlfw(){
    /// GLEW Initialization
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
}

void initViewport(int width, int height){
    glViewport(0 , 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void closeWindow(GLFWwindow* window){
    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


static void PrintShaderErrors(GLuint id, const std::string label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}

inline std::string loadShaderFromFile(const std::string& fileName)
{
   std::ifstream file(fileName);
   if (!file)
   {
      std::cout << "Cannot load file: " << fileName << std::endl;
      return "";
   }

   std::stringstream code;
   code << file.rdbuf();
   file.close();

   return code.str();
}

inline GLuint loadShader(const std::string& vertex, const std::string& fragment)
{
   GLint result;
   std::string vertexShader = loadShaderFromFile(vertex);
   const char* vertexShaderRaw = vertexShader.c_str();
   GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
   glCompileShader(vshaderId);
   glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(vshaderId, "Vertex shader");
      return -1;
   }

   std::string fragmentShader = loadShaderFromFile(fragment);
   const char* fragmentShaderRaw = fragmentShader.c_str();
   GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
   glCompileShader(fshaderId);
   glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(fshaderId, "Fragment shader");
      return -1;
   }

   GLuint shaderId = glCreateProgram();
   glAttachShader(shaderId, vshaderId);
   glAttachShader(shaderId, fshaderId);
   glLinkProgram(shaderId);
   glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(shaderId, "Shader link");
      return -1;
   }
   return shaderId;
}

inline GLuint loadTexture(const std::string& filename)
{
   Image image;
   image.load(filename);

   glEnable(GL_TEXTURE0);
   glActiveTexture(GL_TEXTURE0);

   GLuint texId;
   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image.width(), image.height());
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(),
      GL_RGBA, GL_UNSIGNED_BYTE, image.data());

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   // cleanup
   glBindTexture(GL_TEXTURE_2D, 0);

   return texId;
}
